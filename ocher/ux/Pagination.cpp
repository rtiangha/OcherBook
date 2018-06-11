/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/Pagination.h"

#include "util/Debug.h"
#include "util/Logger.h"

#define LOG_NAME "ocher.pagination"


Pagination::Pagination() :
    m_numPages(0)
{
}

Pagination::~Pagination()
{
    flush();
}

void Pagination::flush()
{
    m_numPages = 0;
    for (auto chunk : m_chunks) {
        delete chunk;
    }
    m_chunks.clear();
}

void Pagination::set(unsigned int pageNum, unsigned int layoutOffset, unsigned int strOffset /* TODO attrs */)
{
    ASSERT(pageNum <= m_numPages);
    unsigned int chunk = pageNum / pagesPerChunk;
    if (pageNum == m_numPages) {
        if (chunk == m_chunks.size()) {
            m_chunks.push_back(new PageMapping[pagesPerChunk]);
        }
    }
    auto mapping = m_chunks[chunk];
    mapping += pageNum % pagesPerChunk;
    if (mapping->layoutOffset != layoutOffset || mapping->strOffset != strOffset) {
        mapping->layoutOffset = layoutOffset;
        mapping->strOffset = strOffset;
        if (m_numPages <= pageNum)
            m_numPages = pageNum + 1;
        Log::debug(LOG_NAME, "set page %u breaks at layoutOffset %u strOffset %u", pageNum, layoutOffset, strOffset);
    }
}

bool Pagination::get(unsigned int pageNum, unsigned int* layoutOffset, unsigned int* strOffset /* TODO attrs */)
{
    Log::debug(LOG_NAME, "get page %u", pageNum);

    if (pageNum > m_numPages)
        return false;
    unsigned int chunk = pageNum / pagesPerChunk;
    if (chunk > m_chunks.size()) {
        return false;
    }
    auto mapping = m_chunks[chunk];
    mapping += pageNum % pagesPerChunk;
    *layoutOffset = mapping->layoutOffset;
    *strOffset = mapping->strOffset;
    Log::debug(LOG_NAME, "found page %u breaks at layoutOffset %u strOffset %u", pageNum, *layoutOffset, *strOffset);
    return true;
}

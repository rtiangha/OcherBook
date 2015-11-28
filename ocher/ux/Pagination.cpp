/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/ux/Pagination.h"
#include "ocher/util/Debug.h"
#include "ocher/util/Logger.h"

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
    for (auto it = m_pages.begin(); it < m_pages.end(); ++it) {
        delete *it;
    }
    m_pages.clear();
}

void Pagination::set(unsigned int pageNum, unsigned int layoutOffset, unsigned int strOffset /* TODO attrs */)
{
    ASSERT(pageNum <= m_numPages);
    unsigned int chunk = pageNum / pagesPerChunk;
    if (pageNum == m_numPages) {
        if (chunk == m_pages.size()) {
            m_pages.push_back(new PageMapping[pagesPerChunk]);
        }
    }
    struct PageMapping *mapping = (struct PageMapping *)m_pages[chunk];
    mapping += pageNum % pagesPerChunk;
    if (mapping->layoutOffset != layoutOffset || mapping->strOffset != strOffset) {
        mapping->layoutOffset = layoutOffset;
        mapping->strOffset = strOffset;
        if (m_numPages <= pageNum)
            m_numPages = pageNum + 1;
        Log::debug(LOG_NAME, "set page %u breaks at layoutOffset %u strOffset %u", pageNum, layoutOffset, strOffset);
    }
}

bool Pagination::get(unsigned int pageNum, unsigned int *layoutOffset, unsigned int *strOffset /* TODO attrs */)
{
    Log::debug(LOG_NAME, "get page %u", pageNum);

    if (pageNum > m_numPages)
        return false;
    unsigned int chunk = pageNum / pagesPerChunk;
    if (chunk > m_pages.size()) {
        return false;
    }
    struct PageMapping *mapping = (struct PageMapping *)m_pages[chunk];
    mapping += pageNum % pagesPerChunk;
    *layoutOffset = mapping->layoutOffset;
    *strOffset = mapping->strOffset;
    Log::debug(LOG_NAME, "found page %u breaks at layoutOffset %u strOffset %u", pageNum, *layoutOffset, *strOffset);
    return true;
}

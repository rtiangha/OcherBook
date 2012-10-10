#ifndef OCHER_META_H
#define OCHER_META_H

#include "clc/data/Buffer.h"

#include "ocher/ux/Pagination.h"


enum Encoding {
    OCHER_ENC_UNKNOWN,
    OCHER_ENC_UTF8,
    OCHER_ENC_UTF16BE,
    OCHER_ENC_UTF16LE,
    OCHER_ENC_UTF32BE,
    OCHER_ENC_UTF32LE,
};

enum Fmt {
    OCHER_FMT_UNKNOWN,
    OCHER_FMT_EPUB,
    OCHER_FMT_TEXT,
    OCHER_FMT_HTML,
};

// TODO: per-user
class BookRecord
{
public:
    BookRecord() : lastPage(0) {}

    int lastPage;  // TODO:  can't use page number; varies by device/settings

    // TODO: dates
    // TODO: highlights
    // TODO: annotations
    // TODO: notes
};

class Meta
{
public:
    Meta();

    static const char* fmtToStr(Fmt fmt);

    Fmt format;
    Encoding encoding;

    clc::Buffer relPath;  // TODO: for now, full path

    clc::Buffer author;
    clc::Buffer title;
    clc::Buffer language;

    clc::Buffer icon;

    Pagination m_pagination;
    unsigned int pages; // is this even meaningful?

    BookRecord record;   // TODO:  per-user
};

#endif


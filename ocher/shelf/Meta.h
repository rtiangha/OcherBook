#ifndef OCHER_META_H
#define OCHER_META_H

#include "clc/data/Buffer.h"

enum Fmt {
    OCHER_FMT_UNKNOWN,
    OCHER_FMT_EPUB,
    OCHER_FMT_TEXT,
    OCHER_FMT_HTML,
};

// TODO: per-user
class BookRecord
{
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

    void detect(const char* file);

    Fmt format;

    clc::Buffer relPath;  // TODO: for now, full path

    clc::Buffer author;
    clc::Buffer title;
    clc::Buffer language;

    clc::Buffer icon;

    unsigned int pages; // is this even meaningful?

    BookRecord record;   // TODO:  per-user
};

#endif


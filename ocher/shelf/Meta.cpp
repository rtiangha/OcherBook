/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/fmt/Format.h"
#include "ocher/shelf/Meta.h"
#include "ocher/util/File.h"

#include <time.h>


void BookRecord::touch()
{
    touched = time(NULL);
}

Meta::Meta() :
    format(OCHER_FMT_UNKNOWN),
    encoding(OCHER_ENC_UNKNOWN),
    pages(0)
{
}

const char *Meta::fmtToStr(Fmt fmt)
{
    switch (fmt) {
    case OCHER_FMT_EPUB:
        return "EPUB";
    case OCHER_FMT_TEXT:
        return "TEXT";
    case OCHER_FMT_HTML:
        return "HTML";
    default:
        return "";
    }
}

void loadMeta(Meta *meta)
{
    switch (meta->format) {
    case OCHER_FMT_TEXT: {
        Text text(meta->relPath);
        // TODO
        meta->title = meta->relPath;
        meta->author = "?";
        break;
    }
    case OCHER_FMT_EPUB: {
#ifdef OCHER_EPUB
        Epub epub(meta->relPath);
        meta->title = epub.m_title;
        meta->author = epub.m_author;
#endif
        break;
    }
    default:
        meta->title = meta->relPath;
        meta->author = "?";
        break;
    }

    // TODO:  Hack: use filename until epub parser can pull out the real title.
    const char *path = strrchr(meta->relPath.c_str(), '/');
    meta->title = path ? path + 1 : meta->relPath.c_str();
}

unsigned int Meta::percentRead()
{
    if (pages == 0)
        return 0;
    if (record.activePage > pages)
        return 100;
    return record.activePage * 100 / pages;
}

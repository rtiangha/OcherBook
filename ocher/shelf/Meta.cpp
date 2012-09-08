#include "clc/storage/File.h"

#include "ocher/shelf/Meta.h"


Meta::Meta() :
    format(OCHER_FMT_UNKNOWN),
    encoding(OCHER_ENC_UNKNOWN),
    pages(0)
{
}

const char* Meta::fmtToStr(Fmt fmt)
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


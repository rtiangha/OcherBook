#include "clc/storage/File.h"

#include "ocher/shelf/Meta.h"


Meta::Meta() :
    format(OCHER_FMT_UNKNOWN),
    pages(0)
{
}


void Meta::detect(const char* file)
{
    clc::File f(file);
    char buf[2];
    if (f.read(buf, 2) == 2 && buf[0] == 'P' && buf[1] == 'K') {
        format = OCHER_FMT_EPUB;
    // TODO:  TEXT, ...
    } else {
        format = OCHER_FMT_UNKNOWN;
    }

    if (format != OCHER_FMT_UNKNOWN) {
        relPath = file;
    }
}


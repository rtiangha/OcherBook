#ifndef OCHER_FMT_H
#define OCHER_FMT_H

#include "clc/data/Buffer.h"

#include "ocher/shelf/Meta.h"


/**
 *  Base class for file-format readers.
 */
class Format
{
public:
    virtual void loadMeta(Meta* meta) {}

    clc::Buffer m_title;
    clc::Buffer m_author;
};

Fmt detectFormat(const char* file);

#endif


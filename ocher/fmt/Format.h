/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FMT_H
#define OCHER_FMT_H

#include "shelf/Meta.h"

#include <string>


/** Base class for file-format readers.
 */
class Format {
public:
    virtual ~Format()
    {
    }

    virtual void loadMeta(Meta*)
    {
    }

    std::string m_title;
    std::string m_author;
};

Fmt detectFormat(const char* file, Encoding* encoding);

#include "fmt/epub/Epub.h"
#include "fmt/text/Text.h"

#endif

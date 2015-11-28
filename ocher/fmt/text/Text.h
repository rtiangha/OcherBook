/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FMT_TEXT_H
#define OCHER_FMT_TEXT_H

#include "ocher/fmt/Format.h"


class Text : public Format {
public:
    Text(const std::string &filename);

    virtual ~Text()
    {
    }

    std::string m_text;
};

#endif

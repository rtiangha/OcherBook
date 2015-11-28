/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_CSS_H
#define OCHER_CSS_H

#include <libcss/libcss.h>

#include <string>

class CssParser {
public:
    CssParser();
    ~CssParser();

    void parseString(std::string &b);

    css_stylesheet *sheet;
};

#endif

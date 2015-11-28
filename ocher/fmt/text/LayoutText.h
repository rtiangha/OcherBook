/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FMT_TEXT_LAYOUT_H
#define OCHER_FMT_TEXT_LAYOUT_H

#include "ocher/fmt/Layout.h"


class Text;

class LayoutText : public Layout {
public:
    LayoutText(Text *text);

protected:
    Text *m_text;
};

#endif

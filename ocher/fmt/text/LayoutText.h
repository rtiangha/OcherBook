/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FMT_TEXT_LAYOUT_H
#define OCHER_FMT_TEXT_LAYOUT_H

#include "fmt/Layout.h"
#include "util/stdex.h"

#include <memory>

class Text;

class LayoutText {
public:
    LayoutText(Text* text);

    std::unique_ptr<Layout> finish();

protected:
    Text* m_text;
    std::unique_ptr<Layout> m_layout = make_unique<Layout>();
};

#endif

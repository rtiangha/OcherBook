/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FMT_EPUB_LAYOUT_H
#define OCHER_FMT_EPUB_LAYOUT_H

#include "fmt/Layout.h"

#include <mxml.h>

class Epub;

class LayoutEpub : public Layout {
public:
    LayoutEpub(Epub* epub) :
        m_epub(epub)
    {
    }

#if 1
    void append(mxml_node_t* tree);
#else
    void append(std::string& html);
#endif

protected:
    void processNode(mxml_node_t* node);
    void processSiblings(mxml_node_t* node);

    Epub* m_epub;
};

#endif

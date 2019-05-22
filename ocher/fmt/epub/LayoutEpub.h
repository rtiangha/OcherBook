/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FMT_EPUB_LAYOUT_H
#define OCHER_FMT_EPUB_LAYOUT_H

#include "fmt/Layout.h"
#include "util/stdex.h"

#include <mxml.h>

#include <memory>

class Epub;

class LayoutEpub {
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

    std::unique_ptr<Layout> finish();

protected:
    void processNode(mxml_node_t* node);
    void processSiblings(mxml_node_t* node);

    Epub* m_epub;
    std::unique_ptr<Layout> m_layout = make_unique<Layout>();
};

#endif

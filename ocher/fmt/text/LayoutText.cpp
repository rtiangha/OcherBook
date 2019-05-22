/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "fmt/text/LayoutText.h"
#include "fmt/text/Text.h"


LayoutText::LayoutText(Text* text) :
    m_text(text)
{
    bool sawNl = false;
    unsigned int n = m_text->m_text.size();
    const char* raw = m_text->m_text.data();

    for (unsigned int i = 0; i < n; ++i) {
        // \n\n means real line break; otherwise reflow text
        if (raw[i] == '\n') {
            if (sawNl) {
                m_layout->outputNl();
                m_layout->outputBr();
                continue;
            } else
                sawNl = true;
        } else if (raw[i] == '\f') {
            m_layout->outputPageBreak();
            continue;
        } else {
            sawNl = false;
        }
        m_layout->outputChar(raw[i]);
    }
    m_layout->flushText();
}

std::unique_ptr<Layout> LayoutText::finish()
{
    m_layout->finish();
    return std::move(m_layout);
}

/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#define __USE_GNU  // for memrchr

#include "ocher/fmt/Layout.h"
#include "ocher/settings/Options.h"
#include "ocher/ux/Pagination.h"
#include "ocher/ux/cdk/RendererCdk.h"
#include "ocher/util/Debug.h"
#include "ocher/util/Logger.h"

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>


// TODO:  page size
// TODO:  margins (not the same as margins for fb?)

RendererCdk::RendererCdk(WINDOW *scr, CDKSCREEN *screen) :
    m_scr(scr),
    m_screen(screen),
    m_x(0),
    m_y(0),
    ai(1)
{
    getmaxyx(m_scr, m_height, m_width);
}

void RendererCdk::enableUl()
{
    // TODO
}

void RendererCdk::disableUl()
{
    // TODO
}

void RendererCdk::enableEm()
{
    // TODO
}

void RendererCdk::disableEm()
{
    // TODO
}

void RendererCdk::pushAttrs()
{
    a[ai + 1] = a[ai];
    ai++;
}

void RendererCdk::applyAttrs(int i)
{
    if (a[ai].ul && !a[ai - i].ul) {
        enableUl();
    } else if (!a[ai].ul && a[ai - i].ul) {
        disableUl();
    }

    if (a[ai].em && !a[ai - i].em) {
        enableEm();
    } else if (!a[ai].em && a[ai - i].em) {
        disableEm();
    }
}

void RendererCdk::popAttrs()
{
    ai--;
    applyAttrs(-1);
}

int RendererCdk::outputWrapped(std::string *b, unsigned int strOffset, bool doBlit)
{
    unsigned int len = b->size();
    const unsigned char *start = (const unsigned char *)b->data();
    const unsigned char *p = start;

    ASSERT(strOffset <= len);
    len -= strOffset;
    p += strOffset;

    do {
        unsigned int w = m_width - m_x;

        // If at start of line, eat spaces
        if (m_x == 0) {
            while (*p != '\n' && isspace(*p)) {
                ++p;
                --len;
            }
        }

        // How many chars should go out on this line?
        const unsigned char *nl = 0;
        unsigned int n = w;
        if (w >= len) {
            n = len;
            nl = (const unsigned char *)memchr(p, '\n', n);
        } else {
            nl = (const unsigned char *)memchr(p, '\n', n);
            if (!nl) {
                // don't break words
                if (!isspace(*(p + n - 1)) && !isspace(*(p + n))) {
                    unsigned char *space = (unsigned char *)memrchr(p, ' ', n);
                    if (space) {
                        nl = space;
                    }
                }
            }
        }
        if (nl)
            n = nl - p;

        if (doBlit) {
            mvwaddnstr(m_scr, m_y, m_x, (const char *)p, n);
        }
        p += n;
        len -= n;
        m_x += n;
        if (nl || m_x >= m_width - 1) {
            m_x = 0;
            m_y++;
            if (nl) {
                p++;
                len--;
            }
            if (m_height > 0 && m_y + 1 >= m_height) {
                return p - start;
            }
        }
    } while (len > 0);
    return -1;
}


int RendererCdk::render(Pagination *pagination, unsigned int pageNum, bool doBlit)
{
    Log::info("ocher.render.cdk", "render page %u %u", pageNum, doBlit);

    m_x = 0;
    m_y = 0;
    if (m_height) {
        wclear(m_scr);
    }

    unsigned int layoutOffset;
    unsigned int strOffset;
    if (!pageNum) {
        layoutOffset = 0;
        strOffset = 0;
    } else if (!pagination->get(pageNum - 1, &layoutOffset, &strOffset)) {
        // Previous page not already paginated?
        // Perhaps at end of book?
        Log::error("ocher.renderer.cdk", "page %u not found", pageNum);
        return -1;
    }

    const unsigned int N = m_layout.size();
    const char *raw = m_layout.data();
    ASSERT(layoutOffset < N);
    for (unsigned int i = layoutOffset; i < N; ) {
        ASSERT(i + 2 <= N);
        uint16_t code = *(uint16_t *)(raw + i);
        i += 2;

        unsigned int opType = (code >> 12) & 0xf;
        unsigned int op = (code >> 8) & 0xf;
        unsigned int arg = code & 0xff;
        switch (opType) {
        case Layout::OpPushTextAttr:
            Log::debug("ocher.render.cdk", "OpPushTextAttr");
            switch (op) {
            case Layout::AttrBold:
                pushAttrs();
                a[ai].b = 1;
                if (doBlit)
                    applyAttrs(1);
                break;
            case Layout::AttrUnderline:
                pushAttrs();
                a[ai].ul = 1;
                if (doBlit)
                    applyAttrs(1);
                break;
            case Layout::AttrItalics:
                pushAttrs();
                a[ai].em = 1;
                if (doBlit)
                    applyAttrs(1);
                break;
            case Layout::AttrSizeRel:
                pushAttrs();
                break;
            case Layout::AttrSizeAbs:
                pushAttrs();
                break;
            default:
                Log::error("ocher.render.cdk", "unknown OpPushTextAttr");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpPushLineAttr:
            Log::debug("ocher.render.cdk", "OpPushLineAttr");
            switch (op) {
            case Layout::LineJustifyLeft:
                break;
            case Layout::LineJustifyCenter:
                break;
            case Layout::LineJustifyFull:
                break;
            case Layout::LineJustifyRight:
                break;
            default:
                Log::error("ocher.render.cdk", "unknown OpPushLineAttr");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpCmd:
            switch (op) {
            case Layout::CmdPopAttr:
                Log::trace("ocher.render.cdk", "OpCmd CmdPopAttr");
                if (arg == 0)
                    arg = 1;
                while (arg--) {
                    popAttrs();
                }
                break;
            case Layout::CmdOutputStr: {
                Log::trace("ocher.render.cdk", "OpCmd CmdOutputStr");
                ASSERT(i + sizeof(std::string *) <= N);
                std::string *str = *(std::string **)(raw + i);
                ASSERT(strOffset <= str->size());
                int breakOffset = outputWrapped(str, strOffset, doBlit);
                strOffset = 0;
                if (breakOffset >= 0) {
                    pagination->set(pageNum, i - 2, breakOffset);
                    if (doBlit) {
                        refreshCDKScreen(m_screen);
                    }
                    Log::debug("ocher.renderer.cdk", "page %u break", pageNum);
                    return 0;
                }
                i += sizeof(std::string *);
                break;
            }
            case Layout::CmdForcePage:
                Log::trace("ocher.render.cdk", "OpCmd CmdForcePage");
                break;
            default:
                Log::error("ocher.render.cdk", "unknown OpCmd");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpSpacing:
            break;
        case Layout::OpImage:
            break;
        default:
            Log::error("ocher.render.cdk", "unknown op type");
            ASSERT(0);
            break;

        }
        ;
    }
    Log::debug("ocher.renderer.cdk", "page %u done", pageNum);
    refreshCDKScreen(m_screen);
    return 1;
}

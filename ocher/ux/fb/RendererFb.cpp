/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "ux/fb/RendererFb.h"

#include "Container.h"
#include "fmt/Layout.h"
#include "settings/Settings.h"
#include "util/Debug.h"
#include "util/Logger.h"
#include "util/Stopwatch.h"
#include "ux/Pagination.h"
#include "ux/fb/FrameBuffer.h"

#include <cctype>

#define LOG_NAME "ocher.render.fb"
#ifdef DEBUG
#define CPS_STATS
#endif


RendererFb::RendererFb(FrameBuffer* fb, FontEngine* fe) :
    m_fb(fb),
    m_fe(fe),
    m_fc(fb->ppi()),
    m_settings(g_container->settings),
    m_penX(m_settings.marginLeft),
    m_penY(m_settings.marginTop),
    ai(0)
{
}

bool RendererFb::init()
{
    a[ai] = {};
    a[ai].pts = m_settings.fontPoints;
    return true;
}

void RendererFb::pushAttrs()
{
    a[ai + 1] = a[ai];
    ai++;
}

void RendererFb::popAttrs()
{
    ai--;
}

int RendererFb::outputWrapped(std::string* b, unsigned int strOffset, bool doBlit)
{
    unsigned int len = b->size();
    const char* start = b->data();
    const char* p = start;

    ASSERT(strOffset <= len);
    len -= strOffset;
    p += strOffset;

    if (m_penY == m_settings.marginTop) {
        while (*p == '\n') {
            ++p;
            --len;
        }
    }
    if (m_penX == m_settings.marginLeft) {
        if (m_penY >= (int)m_fb->yres() - m_settings.marginBottom - m_fc.descender()) {
            return 0;
        }
    }

    bool wordWrapped = false;
    int xres = m_fb->xres();
    do {
        // If at start of line, eat spaces
        if (m_penX == m_settings.marginLeft) {
            while (*p != '\n' && isspace(*p)) {
                ++p;
                --len;
            }
        }

        if (*p != '\n') {
            // Where is the next word break?
            unsigned int w = 0;
            while (*(p + w) && !isspace(*(p + w))) {
                ++w;
            }
            if (*(p + w))
                ++w;

            Rect bbox;
            bbox.x = m_penX;
            bbox.y = m_penY;
            auto glyphs = m_fe->calculateGlyphs(m_fc, p, w, &bbox);
            if (m_penX + bbox.w >= xres - m_settings.marginRight &&
                bbox.w <= xres - m_settings.marginRight - m_settings.marginLeft) {
                bbox.x = m_penX = m_settings.marginLeft;
                m_penY += m_fc.lineHeight();
                bbox.y = m_penY;
            }
            if (m_penY >= (int)m_fb->yres() - m_settings.marginBottom - m_fc.descender())
                return p - start;
            bbox.y -= m_fc.ascender();
            bbox.h = m_fc.lineHeight();
            /* TODO save bounding box + glyphs for selection */

            // Fits; render it and advance
            if (doBlit) {
                Pos pos(m_penX, m_penY);
                m_fe->blitGlyphs(glyphs, &pos);
                m_penX = pos.x;
                m_penY = pos.y;
            } else {
                for (auto glyph : glyphs) {
                    m_penX += glyph->advanceX;
                    m_penY += glyph->advanceY;
                }
            }
            p += w;
            len -= w;
        }

        // Word-wrap or hard linefeed, but avoid the two back-to-back.
        if ((*p == '\n' && !wordWrapped) || m_penX >= xres - 1 - m_settings.marginRight) {
            m_penX = m_settings.marginLeft;
            m_penY += m_fc.lineHeight();
            if (*p == '\n') {
                p++;
                len--;
            } else {
                wordWrapped = true;
            }
            if (m_penY >= (int)m_fb->yres() - m_settings.marginBottom - m_fc.descender())
                return p - start;
        } else {
            if (*p == '\n') {
                p++;
                len--;
            }
            wordWrapped = false;
        }
    } while (len > 0);
    return -1;  // think of this as "failed to cross page boundary"
}

void RendererFb::applyAttrs()
{
    FontFace face;
    face.points    = a[ai].pts;
    face.bold      = a[ai].b;
    face.underline = a[ai].ul;
    face.italic    = a[ai].em;
    m_fc.apply(*m_fe, face);
}

int RendererFb::render(Pagination* pagination, unsigned int pageNum, bool doBlit)
{
    Log::info(LOG_NAME, "%s page %u of %u", doBlit ? "render" : "layout", pageNum, pagination->numPages());

    // TODO reapply font settings before relying on font metrics
    m_penX = m_settings.marginLeft;
    m_penY = m_settings.marginTop + m_fc.ascender();
    if (doBlit)
        m_fb->clear();

    unsigned int layoutOffset;
    unsigned int strOffset;
    if (!pageNum) {
        layoutOffset = 0;
        strOffset = 0;
    } else if (!pagination->get(pageNum - 1, &layoutOffset, &strOffset)) {
        // Previous page not already paginated?
        // Perhaps at end of book?
        Log::error(LOG_NAME, "page %u not found", pageNum);
        return -1;
    }

    int r = 1;
    const unsigned int N = m_layout->size();
    const auto raw = m_layout->data();
    ASSERT(layoutOffset <= N);

    applyAttrs();
#ifdef CPS_STATS
    Stopwatch sw;
    unsigned int chars = 0;
#endif
    unsigned int i = layoutOffset;
    while (i < N) {
        ASSERT(i + 2 <= N);
        auto code = *(const uint16_t*)(raw + i);
        i += 2;

        unsigned int opType = (code >> 12) & 0xf;
        unsigned int op = (code >> 8) & 0xf;
        unsigned int arg = code & 0xff;
        switch (opType) {
        case Layout::OpPushTextAttr:
            Log::trace(LOG_NAME, "OpPushTextAttr");
            switch (op) {
            case Layout::AttrBold:
                pushAttrs();
                Log::trace(LOG_NAME, "font push bold");
                a[ai].b = 1;
                break;
            case Layout::AttrUnderline:
                pushAttrs();
                Log::trace(LOG_NAME, "font push underline");
                a[ai].ul = 1;
                break;
            case Layout::AttrItalics:
                pushAttrs();
                Log::trace(LOG_NAME, "font push italics");
                a[ai].em = 1;
                break;
            case Layout::AttrSizeRel:
                pushAttrs();
                Log::trace(LOG_NAME, "font push rel %d", (int)arg);
                a[ai].pts += (int)arg;
                break;
            case Layout::AttrSizeAbs:
                pushAttrs();
                Log::trace(LOG_NAME, "font push abs %d", (int)arg);
                a[ai].pts = (int)arg;
                break;
            default:
                Log::error(LOG_NAME, "unknown OpPushTextAttr");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpPushLineAttr:
            Log::trace(LOG_NAME, "OpPushLineAttr");
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
                Log::error(LOG_NAME, "unknown OpPushLineAttr");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpCmd:
            switch (op) {
            case Layout::CmdPopAttr:
                Log::trace(LOG_NAME, "OpCmd CmdPopAttr");
                if (arg == 0)
                    arg = 1;
                while (arg--) {
                    popAttrs();
                }
                Log::trace(LOG_NAME, "font pop (%d pts%s%s)", a[ai].pts, a[ai].b ? " bold" : "", a[ai].em ? " italics" : "");
                applyAttrs();
                break;
            case Layout::CmdOutputStr: {
                Log::trace(LOG_NAME, "OpCmd CmdOutputStr");
                ASSERT(i + sizeof(std::string*) <= N);
                applyAttrs();
                auto str = *(std::string* const*)(raw + i);
                ASSERT(strOffset <= str->size());
#ifdef CPS_STATS
                chars += str->length();          // miscounts UTF8...
#endif
                Log::trace(LOG_NAME, "output (%d pts%s%s) %zu bytes", a[ai].pts, a[ai].b ? " bold" : "",
                        a[ai].em ? " italics" : "", str->length());
                int breakOffset = outputWrapped(str, strOffset, doBlit);
                strOffset = 0;
                if (breakOffset >= 0) {
                    pagination->set(pageNum, i - 2, breakOffset);
                    r = 0;
                    goto done;
                }
                i += sizeof(std::string*);
                break;
            }
            case Layout::CmdForcePage:
                Log::trace(LOG_NAME, "OpCmd CmdForcePage");
                pagination->set(pageNum, i, 0);
                r = 0;
                goto done;
            default:
                Log::error(LOG_NAME, "unknown OpCmd");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpSpacing:
            break;
        case Layout::OpImage:
            break;
        default:
            Log::error(LOG_NAME, "unknown op type");
            ASSERT(0);
            break;
        }
        ;
    }
done:
#ifdef CPS_STATS
    uint64_t usec = sw.elapsedUSec();
    static unsigned int totalChars = 0;
    static uint64_t totalUSec = 0;
    totalChars += chars;
    totalUSec += usec;
    unsigned int ms = usec / 1000;
    if (ms == 0)
        ms = 1;
    Log::debug(LOG_NAME, "page %u %s; %u chars in %u ms, %u cps, avg %u cps", pageNum,
            r == 0 ? "break" : "done",
            chars, ms, chars * 1000 / ms,
            (unsigned int)(totalChars * 1000 / ((totalUSec < 1000 ? 1000 : totalUSec) / 1000)));
#endif
    return r;
}

/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fd/RendererFd.h"

#include "Container.h"
#include "fmt/Layout.h"
#include "settings/Options.h"
#include "util/Debug.h"
#include "util/Logger.h"
#include "ux/Pagination.h"

#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define LOG_NAME "ocher.render.fd"


// TODO:  margins (not the same as margins for fb?)
// TODO:  if isatty: SIGWINCH

RendererFd::RendererFd() :
    m_isTty(false),
    m_fd(-1),
    m_width(80),
    m_height(0),
    m_x(0),
    m_y(0),
    ai(1)
{
}

bool RendererFd::init()
{
    m_fd = g_container.options->inFd;
    if (isatty(m_fd) == -1) {
        if (errno == EBADF) {
            Log::error(LOG_NAME, "bad file descriptor");
            return false;
        } else {
            m_isTty = 0;
        }
    } else {
        m_isTty = 1;
        struct winsize win;
        if (ioctl(0, TIOCGWINSZ, &win) == 0) {
            m_width = win.ws_col;
            m_height = win.ws_row;
        }
    }
    return true;
}

void RendererFd::clearScreen()
{
    write(m_fd, "\033E", 2);
}

void RendererFd::enableUl()
{
    write(m_fd, "\x1b[4m", 4);
}

void RendererFd::disableUl()
{
    write(m_fd, "\x1b[24m", 5);
}

void RendererFd::enableEm()
{
    write(m_fd, "\x1b[1m", 4);
}

void RendererFd::disableEm()
{
    write(m_fd, "\x1b[22m", 5);
}

void RendererFd::pushAttrs()
{
    a[ai + 1] = a[ai];
    ai++;
}

void RendererFd::applyAttrs(int i)
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

void RendererFd::popAttrs()
{
    ai--;
    applyAttrs(-1);
}

int RendererFd::outputWrapped(std::string* b, unsigned int strOffset, bool doBlit)
{
    unsigned int len = b->size();
    auto start = (const unsigned char*)b->data();
    auto p = start;

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
        const unsigned char* nl = nullptr;
        unsigned int n = w;
        if (w >= len) {
            n = len;
            nl = (const unsigned char*)memchr(p, '\n', n);
        } else {
            nl = (const unsigned char*)memchr(p, '\n', n);
            if (!nl) {
                // don't break words
                if (!isspace(*(p + n - 1)) && !isspace(*(p + n))) {
                    auto space = (unsigned char*)memrchr(p, ' ', n);
                    if (space) {
                        nl = space;
                    }
                }
            }
        }
        if (nl)
            n = nl - p;

        if (doBlit)
            write(m_fd, p, n);
        p += n;
        len -= n;
        m_x += n;
        if (nl || m_x >= m_width - 1) {
            if (doBlit)
                write(m_fd, "\n", 1);
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


int RendererFd::render(Pagination* pagination, unsigned int pageNum, bool doBlit)
{
    Log::info(LOG_NAME, "render page %u %u", pageNum, doBlit);

    m_x = 0;
    m_y = 0;
    if (m_height) {
        clearScreen();
    }

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

    const unsigned int N = m_layout.size();
    const char* raw = m_layout.data();
    ASSERT(layoutOffset < N);
    for (unsigned int i = layoutOffset; i < N; ) {
        ASSERT(i + 2 <= N);
        uint16_t code = *(uint16_t*)(raw + i);
        i += 2;

        unsigned int opType = (code >> 12) & 0xf;
        unsigned int op = (code >> 8) & 0xf;
        unsigned int arg = code & 0xff;
        switch (opType) {
        case Layout::OpPushTextAttr:
            Log::debug(LOG_NAME, "OpPushTextAttr");
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
                Log::error(LOG_NAME, "unknown OpPushTextAttr");
                ASSERT(0);
                break;
            }
            break;
        case Layout::OpPushLineAttr:
            Log::debug(LOG_NAME, "OpPushLineAttr");
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
                break;
            case Layout::CmdOutputStr: {
                Log::trace(LOG_NAME, "OpCmd CmdOutputStr");
                ASSERT(i + sizeof(std::string*) <= N);
                std::string* str = *(std::string**)(raw + i);
                ASSERT(strOffset <= str->size());
                int breakOffset = outputWrapped(str, strOffset, doBlit);
                strOffset = 0;
                if (breakOffset >= 0) {
                    pagination->set(pageNum, i - 2, breakOffset);
                    Log::debug(LOG_NAME, "page %u break", pageNum);
                    return 0;
                }
                i += sizeof(std::string*);
                break;
            }
            case Layout::CmdForcePage:
                Log::trace(LOG_NAME, "OpCmd CmdForcePage");
                break;
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
    Log::debug(LOG_NAME, "page %u done", pageNum);
    return 1;
}

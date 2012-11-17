#include <stdio.h>

#include "clc/support/Logger.h"

#include "ocher/device/Filesystem.h"
#include "ocher/settings/Options.h"
#include "ocher/shelf/Meta.h"
#include "ocher/ux/fb/FactoryFb.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/fb/Widgets.h"


struct BrowseItem
{
    Rect rect;
    Meta* meta;
};

class BrowseWindow : public Window
{
public:
    BrowseWindow(clc::List* meta);
    ~BrowseWindow();

    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    void freeLayout();
    void layout();
    void drawContent(Rect* rect);
    void drawItem(int i);

protected:
    clc::List* m_meta;
    clc::List m_items;  ///< Items of BrowseItem* displayed on the current page
    unsigned int m_offset;  ///< Offset in m_meta of current page's items
    int8_t m_item;  ///< Index of currently select item (or <0 if none)
    uint8_t m_titlePts;
    uint8_t m_titleHeight;
};

BrowseWindow::BrowseWindow(clc::List* meta) :
    Window(0, 0, g_fb->width(), g_fb->height()),
    m_meta(meta),
    m_item(-1),
    m_titlePts(16)
{
}

BrowseWindow::~BrowseWindow()
{
    freeLayout();
}

int BrowseWindow::evtKey(struct OcherEvent* evt)
{
    clc::Log::debug("ocher.fb.browse", "key");
    int r = -2;
    int n = (int)m_items.size();
    if (evt->subtype == OEVT_KEY_DOWN) {
        r = -1;
        if (evt->key.key == OEVTK_DOWN) {
            clc::Log::debug("ocher.fb.browse", "key down");
            if (m_item+1 < n) {
                m_item++;
                if (m_item > 0)
                    drawItem(m_item-1);
                drawItem(m_item);
            }
        } else if (evt->key.key == OEVTK_UP) {
            clc::Log::debug("ocher.fb.browse", "key up");
            if (m_item > 0) {
                m_item--;
                drawItem(m_item);
                drawItem(m_item+1);
            }
        } else if (evt->key.key == OEVTK_LEFT || evt->key.key == OEVTK_PAGEUP) {
            clc::Log::debug("ocher.fb.browse", "page up");
            // TODO
        } else if (evt->key.key == OEVTK_RIGHT || evt->key.key == OEVTK_PAGEDOWN) {
            clc::Log::debug("ocher.fb.browse", "page down");
            // TODO
        } else if (evt->key.key == OEVTK_ENTER) {
            if (m_item >= 0) {
                clc::Log::debug("ocher.fb.browse", "selected %d", m_item);
                return m_item;
            }
        } else {
            r = -2;
        }
    }
    draw(m_rect.pos());
    return r;
}

int BrowseWindow::evtMouse(struct OcherEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_CLICKED) {
        printf("clicked\n");
        // TODO
    }
    return -2;
}

void BrowseWindow::freeLayout()
{
    for (unsigned int i = 0; i < m_items.size(); ++i) {
        delete (BrowseItem*)m_items.get(i);
    }
}

void BrowseWindow::layout()
{
    freeLayout();

#if 0
    int w = g_fb->width();
    int h = g_fb->height();
    int penX = 0;
    int penY = h*.2;
    int dx, dy;
    uint8_t lineHeight;

    g_ft->setSize(m_titlePts);
    g_ft->renderGlyph('T', false, penX, penY, &dx, &dy, &lineHeight);

    g_fb->setFg(0, 0, 0);
    for (unsigned int i = 0; i < 5 && i < m_meta->size(); ++i) {
        Rect r;
        penX = w*.1;
        r.x = penX;
        r.y = penY;
        r.w = w*.9 - r.x;
        g_fb->hline(r.x, r.y, r.w);
        penY += lineHeight;
        Meta* m = ((Meta*)m_meta->get(i));
        if (g_ft->renderString(m->title.c_str(), true, penX, penY, &dx, &dy, &lineHeight)) {
            penX += dx;
            penY += dy;
        }
        penY += lineHeight;
        r.h = penY - r.y;
    }
#endif
}

void BrowseWindow::drawItem(int i)
{
#if 0 // TODO: or invert later?
    if ((int8_t)i == m_item) {
        g_fb->setFg(0xff, 0xff, 0xff);
        g_fb->setBg(0, 0, 0);
    } else {
        g_fb->setFg(0, 0, 0);
        g_fb->setBg(0xff, 0xff, 0xff);
    }
#endif

    BrowseItem* item = ((BrowseItem*)m_items.get(i));
    int dx, dy;
    uint8_t lineHeight;
    Rect& rect = item->rect;
    Meta* meta = item->meta;
    int penX = rect.x;
    int penY = rect.y;
    g_fb->hline(penX, penY, rect.w);
    penY += m_titleHeight;
    if ((int8_t)i == m_item) {
        if (g_ft->renderString("> ", true, penX, penY, &dx, &dy, &lineHeight)) {
            penX += dx;
            penY += dy;
        }
    }
    if (g_ft->renderString(item->meta->title.c_str(), true, penX, penY, &dx, &dy, &lineHeight)) {
        penX += dx;
        penY += dy;
    }
    penY += lineHeight;
    g_fb->update(&rect, false);
}

void BrowseWindow::drawContent(Rect* rect)
{
    clc::Log::debug("ocher.fb.browse", "draw");
    int w = g_fb->width();
    int h = g_fb->height();
    int penX = 0;
    int penY = h*.2;
    int dx, dy;
    uint8_t lineHeight;

    // TODO
    // "BOOKS" centered

    g_ft->setSize(m_titlePts);
    g_ft->renderGlyph('T', false, penX, penY, &dx, &dy, &m_titleHeight);

    // TODO: multiple pages
    g_fb->setFg(0, 0, 0);
    for (unsigned int i = 0; i < 5 /* TODO */ && i < m_meta->size(); ++i) {
        BrowseItem* item = new BrowseItem;
        m_items.add(item);
        penX = w*.1;
        Rect& rect = item->rect;
        item->meta = ((Meta*)m_meta->get(i));
        rect.x = penX;
        rect.y = penY;
        rect.w = w*.9 - rect.x;
        g_fb->hline(rect.x, rect.y, rect.w);
        penY += m_titleHeight;
        if (g_ft->renderString(item->meta->title.c_str(), true, penX, penY, &dx, &dy, &lineHeight)) {
            penX += dx;
            penY += dy;
        }
        penY += lineHeight;
        rect.h = penY - rect.y;
    }
}


BrowseFb::BrowseFb()
{
}

bool BrowseFb::init()
{
    return true;
}

Meta* BrowseFb::browse(clc::List* meta)
{
    Pos p(0, 0);
    Canvas c;
    c.addChild(new BrowseWindow(meta));
    c.draw(&p);
    int r = g_loop->run(&c);
    if (r < 0)
        return NULL;
    return (Meta*)meta->get(r);
}

void BrowseFb::read(Meta* meta)
{
    Renderer* renderer = uiFactory->getRenderer();

    int& pageNum = meta->record.lastPage;
    while (1) {
        int atEnd = renderer->render(&meta->m_pagination, pageNum, true);

        struct OcherEvent evt;
        int oldPageNum = pageNum;
        while (pageNum == oldPageNum) {
            if (g_loop->wait(&evt) == 0) {
                switch (evt.type) {
                    case OEVT_KEY:
                        if (evt.subtype == OEVT_KEY_DOWN) {
                            if (evt.key.key == OEVTK_HOME) {
                                return;
                            } else if (evt.key.key == OEVTK_LEFT || evt.key.key == OEVTK_UP || evt.key.key == OEVTK_PAGEUP) {
                                if (pageNum > 0)
                                    pageNum--;
                            } else if (evt.key.key == OEVTK_RIGHT || evt.key.key == OEVTK_DOWN || evt.key.key == OEVTK_PAGEDOWN) {
                                if (! atEnd)
                                    pageNum++;
                            }
                        }
                        break;
                    case OEVT_MOUSE:
                        if (evt.subtype == OEVT_MOUSE1_CLICKED || evt.subtype == OEVT_MOUSE1_DOWN) {
                            if (evt.mouse.y > g_fb->height()*.9) {
                                // TODO
                            } else if (evt.mouse.x < g_fb->width()/2) {
                                if (pageNum > 0)
                                    pageNum--;
                            } else {
                                if (!atEnd)
                                    pageNum++;
                            }
                            break;
                        }
                    case OEVT_APP:
                        if (evt.subtype == OEVT_APP_CLOSE)
                            exit(0);
                }
            }
        }
    }
}


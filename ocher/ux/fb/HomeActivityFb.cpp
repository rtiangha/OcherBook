/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/HomeActivityFb.h"

#include "Container.h"
#include "settings/Settings.h"
#include "shelf/Meta.h"
#include "util/Logger.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/UxControllerFb.h"

#define LOG_NAME "ocher.ux.Home"


HomeActivityFb::HomeActivityFb(UxControllerFb* c) :
    ActivityFb(c),
    coverRatio(1.6)
{
    int dx = g_container->settings.smallSpace;
    int dy = g_container->settings.smallSpace;

    books[0].x = m_rect.w / 15;
    books[0].y = m_rect.h / 5;
    books[0].w = m_rect.w / 2.8;
    books[0].h = books[0].w * coverRatio;

    books[1].x = books[0].x + books[0].w + dx;
    books[1].y = m_rect.h / 6;
    books[1].w = m_rect.w / 4;
    books[1].h = books[1].w * coverRatio;

    books[2].x = books[1].x + books[1].w + dx;
    books[2].w = m_rect.w / 5;
    books[2].h = books[2].w * coverRatio;
    books[2].y = books[1].y + books[1].h - books[2].h;

    books[3].x = books[1].x;
    books[3].y = books[1].y + books[1].h + dy;
    books[3].w = books[2].w;
    books[3].h = books[2].h;

    books[4] = books[3];
    books[4].x += books[3].w + dx;
    books[4].w -= 2 * dx;
    books[4].h = books[4].w * coverRatio;

    auto systemBar = make_unique<SystemBar>(m_uxController, g_container->battery);
    systemBar->m_sep = false;
    addChild(std::move(systemBar));

    auto button = make_unique<Button>("Browse all...");
    button->m_flags |= WIDGET_BORDERLESS;
    button->setPos({m_rect.x + m_rect.w - button->rect().w - dx, 575});
    button->pressed.Connect(this, &HomeActivityFb::browseButtonPressed);
    addChild(std::move(button));
}

EventDisposition HomeActivityFb::evtMouse(const struct OcherMouseEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_UP) {
        Pos pos(evt->x, evt->y);
        auto metas = m_uxController->ctx.library.getList();
        for (unsigned int i = 0; i < metas.size() && i < NUM_CLUSTER_BOOKS; i++) {
            Meta* meta = metas[i];
            if (!meta) {
                Log::trace(LOG_NAME, "book %d has no meta", i);
                continue;
            }
            if (books[i].contains(pos)) {
                Log::info(LOG_NAME, "book %d selected %p", i, meta);
                Rect r = books[i];
                r.inset(-2);
                m_fb->roundRect(&r, 3);
                r.inset(-1);
                m_fb->roundRect(&r, 4);
                m_fb->update(&r);
                m_fb->sync();
                m_uxController->ctx.selected = meta;
                m_uxController->setNextActivity(Activity::Type::Read);
                return EventDisposition::Handled;
            }
        }
        // TODO: look at shortlist
    }
    return Widget::evtMouse(evt);
}

void HomeActivityFb::drawContent(const Rect* rect)
{
    Log::debug(LOG_NAME, "draw");

    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
    m_fb->setFg(0, 0, 0);

    auto fe = m_uxController->getFontEngine();
    auto fc = fe->context();
    fc.setPoints(12);
    Rect r;
    Pos pos;
    auto metas = m_uxController->ctx.library.getList();
    for (unsigned int i = 0; i < NUM_CLUSTER_BOOKS; ++i) {
        r = books[i];
        r.inset(-1);
        m_fb->rect(&r);
        r.inset(-1);
        m_fb->roundRect(&r, 1);
        r.inset(2);

        Meta* meta = i < metas.size() ? metas[i] : nullptr;
        uint8_t c = meta ? 0xf0 : 0xd0;
        m_fb->setFg(c, c, c);
        m_fb->fillRect(&r);
        m_fb->setFg(0, 0, 0);
        if (meta) {
            pos.x = 0;
            pos.y = fc.ascender();
            r.inset(2);
            fe->renderString(fc, meta->title.c_str(), meta->title.length(), &pos, &r, FE_YCLIP | FE_WRAP);
        }
    }

    fc.setPoints(18);
    pos.x = 0;
    pos.y = 100;
    fe->renderString(fc, "HOME", 4, &pos, rect, FE_XCENTER);

    // Shortlist
    fc.setPoints(14);
    pos.x = books[0].x;
    pos.y = books[3].y + books[3].h + fc.ascender() + g_container->settings.smallSpace;
    fe->renderString(fc, "Shortlist", 9, &pos, rect, 0);

    pos.y += fc.underlinePos() + g_container->settings.smallSpace;
    m_fb->hline(books[0].x, pos.y, rect->w - books[0].x);
    pos.y++;
    m_fb->hline(books[0].x, pos.y, rect->w - books[0].x);

    pos.x = books[0].x;
    pos.y += g_container->settings.smallSpace;

    {
        auto shortList = m_uxController->ctx.shortList.getList();
        int margin = books[0].x;

        int h = rect->y + rect->h - pos.y - margin;
        int w = h / coverRatio;
        Rect sl(pos.x, pos.y, w, h);
        while (sl.x + sl.w <= rect->w - margin) {
            m_fb->roundRect(&sl, 1);
            sl.inset(1);
            m_fb->roundRect(&sl, 2);
            sl.inset(-1);

            sl.x += sl.w + g_container->settings.smallSpace;

            // TODO
            (void)shortList;
        }
    }

#if 0
    m_fb->byLine(&m_fb->bbox, dim);
    Rect popup(25, 200, 550, 400);
    m_fb->rect(&popup);
    popup.inset(1);
    m_fb->rect(&popup);
    popup.inset(1);
    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(&popup);
#endif
}

void HomeActivityFb::browseButtonPressed(Button&)
{
    Log::info(LOG_NAME, "Browse button pressed");
    m_uxController->setNextActivity(Activity::Type::Library);
}

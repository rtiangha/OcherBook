/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/Widgets.h"

#include "Container.h"
#include "util/Logger.h"
#include "ux/fb/FontEngine.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

#define LOG_NAME "ocher.widgets"

static const unsigned int roundRadius = 1;
static FbScreen* g_screen;

Widget::Widget() :
    m_flags(WIDGET_DIRTY),
    m_screen(g_screen),
    m_parent(nullptr)
{
    assert(g_screen != nullptr);
}

Widget::Widget(int x, int y, unsigned int w, unsigned int h) :
    m_flags(WIDGET_DIRTY),
    m_rect(x, y, w, h),
    m_screen(g_screen),
    m_parent(nullptr)
{
    assert(g_screen != nullptr);
}

Widget& Widget::operator=(Widget&& other)
{
    if (this != &other)
    {
        m_rect = other.m_rect;
        m_screen = other.m_screen;
        m_parent = other.m_parent;
        m_children = std::move(other.m_children);
        other.m_parent = nullptr;
        other.m_screen = nullptr;
    }
    return *this;
}

void Widget::addChild(std::unique_ptr<Widget> child)
{
    auto widget = child.get();
    m_children.push_back(std::move(child));
    widget->onAttached();
    widget->m_parent = this;
    if (!(widget->m_flags & WIDGET_HIDDEN)) {
        widget->invalidate();
    }
}

void Widget::removeChild(Widget* widget)
{
    for (auto it = m_children.begin(); it < m_children.end(); ++it) {
        if (it->get() == widget) {
            widget->onDetached();
            m_children.erase(it);
            widget->m_parent = nullptr;
            break;
        }
    }
}

void Widget::invalidate()
{
    m_flags |= WIDGET_DIRTY;
    for (auto& w : m_children) {
        w->invalidate();
    }
}

void Widget::erase()
{
    m_screen->fb->setFg(0xff, 0xff, 0xff);
    m_screen->fb->fillRect(&m_rect);
}

Rect Widget::drawChildren()
{
    Rect drawn;
    for (auto& w : m_children) {
        if (!(w->m_flags & WIDGET_HIDDEN)) {
            if (w->m_flags & WIDGET_DIRTY) {
                w->draw();
                w->m_flags &= ~WIDGET_DIRTY;
                drawn.unionRect(w->m_rect);
            }
            Rect r = w->drawChildren();
            drawn.unionRect(r);
        }
    }
    return drawn;
}

EventDisposition Widget::evtMouse(const struct OcherMouseEvent* evt)
{
    Pos pos(evt->x, evt->y);
    for (auto& w : m_children) {
        if (w->m_rect.contains(pos)) {
            EventDisposition r = w->evtMouse(evt);
            if (r != EventDisposition::Pass)
                return r;
        }
    }
    return EventDisposition::Pass;
}

Window::Window() :
    Widget(0, 0, 0, 0),
    m_bgColor(0xffffffff),
    m_winflags(0)
{
}

Window::Window(int x, int y, unsigned int w, unsigned int h) :
    Widget(x, y, w, h),
    m_bgColor(0xffffffff),
    m_winflags(0)
{
}

void Window::maximize()
{
    m_rect.x = m_rect.y = 0;
    m_rect.w = m_screen->fb->xres();
    m_rect.h = m_screen->fb->yres();
}

void Window::setTitle(const std::string& title)
{
    m_title = title;
}

void Window::draw()
{
    Rect rect(m_rect);

    drawBorder(&rect);
    drawTitle(&rect);
    drawBg(&rect);
    drawContent(&rect);
    drawChildren();
}

void Window::drawBorder(Rect* rect)
{
    if (!(m_flags & WIDGET_BORDERLESS)) {
        m_screen->fb->setFg(0, 0, 0);
        m_screen->fb->rect(rect);
        rect->inset(1);
    }
}

void Window::drawTitle(Rect* rect)
{
    if (m_winflags & OWF_CLOSE) {
        m_screen->fb->setFg(0, 0, 0);
        m_screen->fb->line(rect->x + rect->w - 12, rect->y + 4, rect->x + rect->w - 4, rect->y + 12);
        m_screen->fb->line(rect->x + rect->w - 4, rect->y + 4, rect->x + rect->w - 12, rect->y + 12);
        rect->y += 12;
        rect->h -= 12;
    }
    if (!m_title.empty()) {
        // TODO  change to Label
    }
}

void Window::drawBg(Rect* rect)
{
    m_screen->fb->setFg(0xff, 0xff, 0xff);
    m_screen->fb->fillRect(rect);
}

void Window::drawContent(const Rect*)
{
}


void Icon::setBitmap(const Bitmap& bitmap)
{
    m_bmp = bitmap;
    m_rect.w = m_bmp.w;
    m_rect.h = m_bmp.h;
}

void Icon::draw()
{
    m_screen->fb->blit(&m_bmp.data[0], m_rect.x, m_rect.y, m_bmp.w, m_bmp.h);
}


Label::Label(const char* label, int points) :
    m_fc(m_screen->fe->context())
{
    setLabel(label, points);
}

Label::Label(Label&& other)
{
    Widget::operator =(std::move(other));

    m_fc = std::move(other.m_fc);
    m_glyphs = std::move(other.m_glyphs);
    m_points = other.m_points;
}

void Label::setLabel(const char* label, int points)
{
    if (points)
        m_fc.setPoints(points);
    m_glyphs = m_screen->fe->calculateGlyphs(m_fc, label, strlen(label), &m_rect);
    invalidate();
}

void Label::draw()
{
    Pos pen{ m_rect.x, m_rect.y + m_fc.bearingY() };
    m_screen->fe->blitGlyphs(m_glyphs, &pen);
}


Button::Button(int x, int y, const char* label) :
    Widget(x, y, 0, 0),
    m_label(label)
{
    setPad(g_container->settings.systemFontPoints / 2);
}

Button::Button(int x, int y, const Bitmap& bitmap) :
    Widget(x, y, bitmap.w, bitmap.h)
{
    setPad(g_container->settings.systemFontPoints / 2);
    setBitmap(bitmap);
}

Button::Button(const char* label, int points) :
    m_label(label, points)
{
    setPad(points / 2);
}

void Button::setBorder(bool border)
{
    if (border)
        m_flags &= ~WIDGET_BORDERLESS;
    else
        m_flags |= WIDGET_BORDERLESS;
    calcSize();
}

void Button::setPad(int points)
{
    m_pad = points * m_screen->fb->ppi() / 72;
    calcSize();
}

void Button::setBitmap(const Bitmap& bitmap)
{
    m_icon.setBitmap(bitmap);
    calcSize();
}

void Button::setLabel(const char* label, int points)
{
    m_label.setLabel(label, points);
    calcSize();
}

// TODO this goes away with real layout
void Button::setPos(const Pos& pos)
{
    m_rect.x = pos.x;
    m_rect.y = pos.y;
    calcSize();
}

// TODO this goes away with real layout
void Button::calcSize()
{
    m_rect.w = m_rect.h = 0;

    if (m_icon.rect().w > 0) {
        m_icon.setPos({m_rect.x + m_pad, m_rect.y + m_pad});
        m_rect.unionRect(m_icon.rect());
    }

    m_label.setPos({m_rect.x + m_rect.w + m_pad, m_rect.y + m_pad});
    m_rect.unionRect(m_label.rect());

    m_rect.w += m_pad;
    m_rect.h += m_pad;
}

void Button::draw()
{
    Rect rect(m_rect);

    drawBorder(&rect);
    drawBg(&rect);
    drawButton(&rect);
    if (m_mouseDown) {
        m_screen->fb->byLine(&rect, dim);
    }
}

// TODO this goes away with real layout
void Button::drawBorder(Rect* rect)
{
    if (!(m_flags & WIDGET_BORDERLESS)) {
        m_screen->fb->setFg(0, 0, 0);
        m_screen->fb->roundRect(rect, roundRadius);
        rect->inset(1);
    }
}

void Button::drawBg(Rect* rect)
{
    m_screen->fb->setFg(0xff, 0xff, 0xff);
    m_screen->fb->fillRect(rect);
}

void Button::drawButton(Rect* rect)
{
    m_icon.draw();
    m_label.draw();
}

EventDisposition Button::evtKey(const struct OcherKeyEvent*)
{
    return EventDisposition::Pass;
}

EventDisposition Button::evtMouse(const struct OcherMouseEvent* evt)
{
    if (evt->subtype == OEVT_MOUSE1_DOWN) {
        m_mouseDown = true;
        invalidate();
    } else if (evt->subtype == OEVT_MOUSE1_UP && m_mouseDown) {
        m_mouseDown = false;
        invalidate();
        pressed();
    }
    return EventDisposition::Handled;
}

void Button::timeoutCb(EV_P_ ev_timer* w, int revents)
{
    // TODO spring the button back up
}

Menu::Menu(int x, int y)
{
    auto tab = make_unique<Button>(x + 1, y + 1, "X");
    tab->setBorder(false);
    tab->pressed.Connect(this, &Menu::tabPressed);
    m_tab = tab.get();
    addChild(std::move(tab));

    m_closedRect = m_tab->rect();
    m_closedRect.inset(-1);
    m_rect = m_closedRect;
}

Menu::~Menu()
{
    m_tab->pressed.Disconnect(this, &Menu::tabPressed);
    for (auto& item : m_items) {
        item.label->pressed.Disconnect(this, &Menu::itemSelected);
    }
}

void Menu::open()
{
    Log::info(LOG_NAME ".menu", "open");
    m_open = true;
    m_screen->invalidate(rect());
    m_rect = m_openRect;

    for (auto& item : m_items) {
        item.label->show();
    }
}

void Menu::close()
{
    Log::info(LOG_NAME ".menu", "close");
    m_open = false;
    m_screen->invalidate(rect());
    m_rect = m_closedRect;

    for (auto& item : m_items) {
        item.label->hide();
    }
}

void Menu::tabPressed()
{
    if (m_open)
        close();
    else
        open();
}

void Menu::itemSelected()
{
    Log::info(LOG_NAME ".menu", "selected");
    // TODO which?
}

void Menu::addItem(const char* text)
{
    const auto& settings = g_container->settings;

    auto label = make_unique<Button>(text, settings.systemFontPoints);
    label->setBorder(false);
    label->setPad(0);
    label->pressed.Connect(this, &Menu::itemSelected);
    label->hide();
    Item item;
    item.label = label.get();
    m_items.push_back(item);
    addChild(std::move(label));

    const auto fc = m_screen->fe->context();
    Rect r{m_tab->rect()};
    Pos pos = r.below();
    pos.x += settings.smallSpace;
    pos.y += settings.smallSpace;
    for (auto& item : m_items) {
        // TODO make all widths the same
        item.label->setPos(pos);
        r.unionRect(item.label->rect());
        pos.y += fc.lineHeight();
    }
    r.grow(settings.smallSpace, fc.descender() + settings.smallSpace);
    m_openRect = r;
}

void Menu::draw()
{
    if (m_open) {
        // TODO draw non-rectangular outline around tab and menu

        auto fb = m_screen->fb;
        fb->setFg(0xff, 0xff, 0xff);
        fb->fillRect(&m_openRect);
        fb->setFg(0, 0, 0);
        fb->rect(&m_openRect);
    }

    drawChildren();
}

Spinner::Spinner() :
    m_state(0),
    m_steps(12),
    m_delayMs(1000)
{
}

Spinner::Spinner(int x, int y, unsigned int w, unsigned int h) :
    Widget(x, y, w, h),
    m_state(0),
    m_steps(12),
    m_delayMs(1000)
{
}

Spinner::~Spinner()
{
    stop();
}

void Spinner::start()
{
    Log::debug(LOG_NAME ".spinner", "start");

    ev_timer_init(&m_timer, timeoutCb, 0, m_delayMs / 1000.0);
    m_timer.data = this;
    ev_timer_start(m_screen->loop.evLoop, &m_timer);
}

void Spinner::timeoutCb(EV_P_ ev_timer* timer, int)
{
    Log::trace(LOG_NAME ".spinner", "timeout");
    auto self = static_cast<Spinner*>(timer->data);

    self->m_state++;
    if (self->m_state >= self->m_steps)
        self->m_state = 0;
    self->invalidate();
}

void Spinner::stop()
{
    Log::debug(LOG_NAME ".spinner", "stop");

    ev_timer_stop(m_screen->loop.evLoop, &m_timer);
}

void Spinner::draw()
{
    Log::trace(LOG_NAME ".spinner", "draw");
    // Numbered like a clock-1, but 0 o'clock is 3 o'clock.
    // Age is time since wave passed by.
    double l1 = m_rect.w / 2.0 * .4;
    double l2 = m_rect.w / 2.0;

    for (unsigned int i = 0; i < m_steps; ++i) {
        unsigned int age = (m_steps - i + m_state) % m_steps;
        uint8_t c = 0xff * age / m_steps;
        double rad = (double)i / (double)m_steps * M_PI * 2;
        int x1 = m_rect.x + m_rect.w / 2 + cos(rad) * l1;
        int y1 = m_rect.y + m_rect.h / 2 + sin(rad) * l1;
        int x2 = m_rect.x + m_rect.w / 2 + cos(rad) * l2;
        int y2 = m_rect.y + m_rect.h / 2 + sin(rad) * l2;
        m_screen->fb->setFg(c, c, c);
        m_screen->fb->line(x1, y1, x2, y2);
        Log::trace(LOG_NAME ".spinner", "%d,%d %d,%d %02x", x1, y1, x2, y2, c);
    }
}


FbScreen::FbScreen(EventLoop& _loop) :
    loop(_loop)
{
    // TODO For now I know the screen is constructed before widgets so this works
    g_screen = this;

    // TODO probe underlying framebuffer for desired refresh rate
    float refreshLatency = 0.25;
    Log::info(LOG_NAME, "Screen refresh every %fs", refreshLatency);
    ev_timer_init(&m_timer, refreshTimeoutCb, refreshLatency, refreshLatency);
    m_timer.data = this;
    ev_timer_start(loop.evLoop, &m_timer);

    ev_prepare_init(&m_evPrepare, readyToIdle);
    m_evPrepare.data = this;
    ev_prepare_start(loop.evLoop, &m_evPrepare);

    ev_check_init(&m_evCheck, waking);
    m_evCheck.data = this;
    ev_check_start(loop.evLoop, &m_evCheck);

    loop.emitEvent.Connect(this, &FbScreen::dispatchEvent);
}

FbScreen::~FbScreen()
{
    loop.emitEvent.Disconnect(this, &FbScreen::dispatchEvent);

    ev_timer_stop(loop.evLoop, &m_timer);
    ev_prepare_stop(loop.evLoop, &m_evPrepare);
    ev_check_stop(loop.evLoop, &m_evCheck);

    g_screen = nullptr;
}

void FbScreen::setFrameBuffer(FrameBuffer* fb_)
{
    fb = fb_;

    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = fb->xres();
    m_rect.h = fb->yres();
}

void FbScreen::setFontEngine(FontEngine* fe_)
{
    fe = fe_;
}

void FbScreen::addChild(std::unique_ptr<Widget> child)
{
    auto widget = child.get();
    m_children.push_back(std::move(child));
    widget->onAttached();
    if (!(widget->m_flags & WIDGET_HIDDEN)) {
        widget->invalidate();
    }
}

void FbScreen::removeChild(Widget* widget)
{
    for (auto it = m_children.begin(); it < m_children.end(); ++it) {
        if (it->get() == widget) {
            widget->onDetached();
            m_children.erase(it);
            break;
        }
    }
}

void FbScreen::invalidate(const Rect& rect) const
{
    for (auto& w : m_children) {
        if (!(w->m_flags & WIDGET_HIDDEN) && rect.intersects(w->rect())) {
            w->invalidate();
        }
    }
}

void FbScreen::update()
{
    Log::trace(LOG_NAME ".screen", "update");

    /* This is good enough for now.  Future things:
     *  - support overlapping children (z order)
     *  - draw only invalid rects (not entire widget)
     */
    Rect drawn;
    for (auto& w : m_children) {
        if (!(w->m_flags & WIDGET_HIDDEN)) {
            if (w->m_flags & WIDGET_DIRTY) {
                w->draw();
                w->m_flags &= ~WIDGET_DIRTY;
                drawn.unionRect(w->rect());
            }
            Rect r = w->drawChildren();
            drawn.unionRect(r);
        }
    }

    if (!drawn.empty()) {
        fb->update(&drawn);
    }
}

/* Theory of interface with libev:
 *
 * - When event loop wakes up to handle new event(s), start a repeating timer.
 * - Purpose of repeating timer is to ensure screen gets periodic updates, even when events continue
 *   to stream in non-stop.
 * - When even loop goes idle, do one last (perhaps the only) update, and cancel the timer.
 */

void FbScreen::refreshTimeoutCb(EV_P_ ev_timer* timer, int)
{
    Log::trace(LOG_NAME ".screen", "timeout");

    static_cast<FbScreen*>(timer->data)->update();
}

void FbScreen::readyToIdle(EV_P_ ev_prepare* p, int)
{
    Log::trace(LOG_NAME ".screen", "readyToIdle");
    auto self = static_cast<FbScreen*>(p->data);

    self->update();
    ev_timer_stop(loop, &self->m_timer);
}

void FbScreen::waking(EV_P_ ev_check* c, int)
{
    Log::trace(LOG_NAME ".screen", "waking");
    auto self = static_cast<FbScreen*>(c->data);

    ev_timer_start(loop, &self->m_timer);
}

void FbScreen::dispatchEvent(const struct OcherEvent* evt)
{
    if (evt->type == OEVT_MOUSE) {
        const auto& mouseEvt = evt->mouse;
        Pos pos(mouseEvt.x, mouseEvt.y);
        for (auto& w : m_children) {
            if (w->rect().contains(pos)) {
                EventDisposition r = w->evtMouse(&mouseEvt);
                if (r != EventDisposition::Pass)
                    break;
            }
        }
    } else if (evt->type == OEVT_KEY) {
        /* TODO one widget has focus */
        for (auto& w : m_children) {
            EventDisposition r = w->evtKey(&evt->key);
            if (r != EventDisposition::Pass)
                break;
        }
    }
}

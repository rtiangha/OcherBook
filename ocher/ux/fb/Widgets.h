/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FB_WIDGETS_H
#define OCHER_FB_WIDGETS_H

#include "ux/Event.h"
#include "ux/fb/bitmap.h"
#include "ux/fb/FontEngine.h"
#include "ux/fb/FrameBuffer.h"
#include "util/stdex.h"

#include <Signal.h>
#include <ev.h>

#include <string>
#include <vector>

#define WIDGET_HIDDEN     1  ///< draw will not be called
#define WIDGET_DIRTY      2
#define WIDGET_BORDERLESS 4


class Widget;
class Window;

/**
 * Represents the drawing area of a FrameBuffer and interacts with an EventLoop.
 */
class FbScreen {
public:
    FbScreen(EventLoop& loop);

    ~FbScreen();

    void setFrameBuffer(FrameBuffer* fb);

    void addChild(std::unique_ptr<Widget> child);

    void removeChild(Widget* child);

    /**
     * Invalidates a portion of the screen, so that affected Widgets will be redrawn.
     */
    void invalidate(const Rect& rect) const;

    void dispatchEvent(const struct OcherEvent*);

    EventLoop& loop;
    FrameBuffer* fb = nullptr;

protected:
    /**
     * Updates the screen:  Causes all invalidated widgets to redraw, and blits to the framebuffer.
     * Done periodically automatically
     */
    void update();

    static void refreshTimeoutCb(EV_P_ ev_timer* w, int revents);
    static void readyToIdle(EV_P_ ev_prepare* p, int revents);
    static void waking(EV_P_ ev_check* c, int revents);

    ev_timer m_timer;
    ev_prepare m_evPrepare;
    ev_check m_evCheck;

    Rect m_rect;
    std::vector<std::unique_ptr<Widget>> m_children;
};


/**
 * Base class for all UI widgets.  Currently all coordinates are absolute,
 * since things don't move around.
 */
class Widget {
public:
    Widget();

    Widget(int x, int y, unsigned int w, unsigned int h);

    virtual ~Widget() = default;

    void addChild(std::unique_ptr<Widget> child);

    void removeChild(Widget* child);

    void hide()
    {
        m_flags |= WIDGET_HIDDEN;
        if (m_parent)
            m_parent->invalidate();
    }

    void show()
    {
        m_flags &= ~WIDGET_HIDDEN;
        invalidate();
    }

    virtual void setPos(int x, int y)
    {
        m_rect.x = x;
        m_rect.y = y;
    }

    void resize(int dx, int dy);

    /**
     * Invalidates the widget, so that it will be redrawn.
     */
    void invalidate();

    /**
     */
    virtual void draw() = 0;

    virtual void onAttached()
    {
    }

    virtual void onDetached()
    {
    }

    const Rect& rect() { return m_rect; }

    Rect drawChildren();

    /**
     * @return -1 handled, -2 pass on, >=0 done
     */
    virtual EventDisposition evtKey(const struct OcherKeyEvent*)
    {
        return EventDisposition::Pass;
    }

    virtual EventDisposition evtMouse(const struct OcherMouseEvent* evt);

    virtual EventDisposition evtApp(const struct OcherAppEvent*)
    {
        return EventDisposition::Pass;
    }

    virtual EventDisposition evtDevice(const struct OcherDeviceEvent*)
    {
        return EventDisposition::Pass;
    }

    unsigned int m_flags;

protected:
    Rect m_rect;
    FbScreen* m_screen;
    Widget* m_parent;
    std::vector<std::unique_ptr<Widget>> m_children;
    // TODO focus
};

#define OWF_CLOSE 1

class Window : public Widget {
public:
    Window();
    Window(int x, int y, unsigned int w, unsigned int h);
    ~Window() = default;

    void setRect(int x, int y, int w, int h)
    {
        m_rect.x = x;
        m_rect.y = y;
        m_rect.w = w;
        m_rect.h = h;
    }

    void draw() final override;
    virtual void drawBorder(Rect* rect);
    virtual void drawTitle(Rect* rect);
    virtual void drawBg(Rect* rect);
    virtual void drawContent(const Rect* rect);

    void setTitle(const std::string& title);
    void maximize();

    uint32_t m_bgColor;
    uint32_t m_winflags;

protected:
    std::string m_title;

    // border style:  none, simple, raised
    // bubble arrow:  none, top, right, bottom, left
    // close
};

class Icon : public Widget {
public:
    Icon() = default;

    Icon(int x, int y, Bitmap& bmp) :
        Widget(x, y, bmp.w, bmp.h),
        m_bmp(bmp)
    {
    }

    void setBitmap(const Bitmap& bitmap);

    void draw() final override;

protected:
    Bitmap m_bmp;
};

class Label : public Widget {
public:
    Label();
    Label(const char* label, int points = 0);

    void setLabel(const char* label, int points = 0);
    void draw() override;

protected:
    FontEngine m_fe;
    std::vector<Glyph*> m_glyphs;
    int m_points = 0;
};

class Button : public Widget {
public:
    Button(int x, int y, const char* label);
    Button(int x, int y, const Bitmap& bitmap);
    Button(const char* label, int points = 0);
    ~Button() = default;

    void setBitmap(const Bitmap& bitmap);
    void setLabel(const char* label, int points = 0);

    void draw() final override;

    Signal0<> pressed;

protected:
    void calcSize();

    virtual void drawBorder(Rect* rect);
    virtual void drawBg(Rect* rect);
    virtual void drawButton(Rect* rect);

    EventDisposition evtKey(const struct OcherKeyEvent*) override;
    EventDisposition evtMouse(const struct OcherMouseEvent*) override;

    Icon m_icon;
    Label m_label;

    bool m_mouseDown = false;

    ev_timer m_timer;
    static void timeoutCb(EV_P_ ev_timer* w, int revents);
};

class Menu : public Widget {
public:
    Menu(int x, int y);
    ~Menu();

    void draw() final override;

    struct Item {
        std::string text;
    };

    Signal1<const Item&> selected;

    std::vector<Item> m_items;

protected:
    Button m_tab;
    bool m_open = false;

    void open();
};

/**
 * @todo listen for powerdown event, stop spinning
 */
class Spinner : public Widget {
public:
    Spinner();
    Spinner(int x, int y, unsigned int w, unsigned int h);
    ~Spinner();

    void setRect(int x, int y, int w, int h)
    {
        m_rect.x = x;
        m_rect.y = y;
        m_rect.w = w;
        m_rect.h = h;
    }

    void start();
    void stop();
    void draw() override;

protected:
    unsigned int m_state;
    unsigned int m_steps;
    unsigned int m_delayMs;

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    ev_timer m_timer;
};

#if 0
class HBox : public Widget {
};
#endif

#if 0
class TextEntry : public Window {
};
#endif

#if 0
class OSKeyboard : public Widget {
};
#endif

#if 0
class Scrubber : public Widget {
    plus / minus vs arrows
    ff arrows
};
#endif

#endif

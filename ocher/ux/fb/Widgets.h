#ifndef OCHER_FB_WIDGETS_H
#define OCHER_FB_WIDGETS_H

#include "clc/data/List.h"
#include "clc/data//Buffer.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/Event.h"

#define WIDGET_HIDDEN 1
#define WIDGET_OWNED  2
#define WIDGET_DIRTY  4


/**
 * Base class for all UI widgets
 */
class Widget
{
public:
    Widget(int x, int y, unsigned int w, unsigned int h);
    virtual ~Widget();

    /**
     * Adds a child (transfers ownership).
     */
    void addChild(Widget* child);
    /**
     * Adds a child (does not transfer ownership).
     */
    void addChild(Widget& child);

    void dirty();
    void hide() { m_flags |= WIDGET_HIDDEN; }
    void show() { m_flags &= ~WIDGET_HIDDEN; }

    /**
     * @param pos  Absolute position to draw at
     * @return Rectangle unioning all dirty rects (this and children), or !valid rect if all clean
     */
    virtual Rect draw(Pos* pos) = 0;

    virtual int evtKey(struct OcherKeyEvent*) { return -2; }
    virtual int evtMouse(struct OcherMouseEvent*) { return -2; }
    virtual int evtApp(struct OcherAppEvent*) { return -2; }
    virtual int evtDevice(struct OcherDeviceEvent*) { return -2; }

    // TODO: abs or rel?
    Rect m_rect;
    unsigned int m_flags;

    virtual void onAttached() {}
    virtual void onDetached() {}

    /**
     * @return -1 handled, -2 pass on, >=0 done
     */
    int _onMouseEvent(struct OcherMouseEvent*);
    void onMouseEvent(struct OcherMouseEvent*);

protected:
    /**
     * @return Rectangle unioning all dirty rects, or !valid rect if all clean
     */
    Rect drawChildren(Pos* pos);

    Widget* m_parent;
    clc::List m_children;
    // TODO focus
};

class Canvas : public Widget
{
public:
    Canvas();
    ~Canvas() {}

    void refresh(bool full=false);
    Rect draw(Pos* pos);

    void loop();
};

class Panel : public Widget
{
public:
    Panel();
    ~Panel() {}

    Rect draw(Pos* pos);
};

#define OWF_CLOSE 1

class Window : public Widget
{
public:
    Window(int x, int y, unsigned int w, unsigned int h);
    ~Window();

    virtual Rect draw(Pos* pos);
    virtual void drawBorder(Rect* rect);
    virtual void drawTitle(Rect* rect);
    virtual void drawBg(Rect* rect);
    virtual void drawContent(Rect* rect);

    void setTitle(const char* title);

    uint32_t m_bgColor;
    uint32_t m_borderColor;
    uint8_t m_borderWidth;
    uint32_t m_winflags;
protected:
    char* m_title;
    // border style:  none, simple, raised
    //bubble arrow:  none, top, right, bottom, left
    //close
};

class Button : public Widget {
public:
    Button(int x, int y, unsigned int w=0, unsigned int h=0);
    ~Button();

    void setLabel(const char* label);

    virtual Rect draw(Pos* pos);
    virtual void drawBorder(Rect* rect);
    virtual void drawLabel(Rect* rect);

protected:
    int evtKey(struct OcherKeyEvent*);
    int evtMouse(struct OcherMouseEvent*);

    int border;
    clc::Buffer m_label;
    //icon;
};

#if 0
class Menu : public Widget
{
    int border;
    int separator;
    items;
    icons;
    title;
    selected;
};
#endif

#if 0
class Spinner : public Widget
{
    state;
    speed;
};
#endif

#if 0
class TextEntry : public Window
{
};
#endif

#if 0
class OSKeyboard : public Widget
{
};
#endif

#if 0
class Label : public Widget
{
    label;
    font;
    size;
    attrs;
};
#endif

class Bitmap
{
public:
    Bitmap(int _w, int _h, const unsigned char* _bmp) : w(_w), h(_h), bmp(_bmp) {}
    int w;
    int h;
    const unsigned char* bmp;
};

class Icon : public Widget
{
public:
    Icon(int x, int y, Bitmap* _bmp) : Widget(x, y, _bmp->w, _bmp->h), bmp(_bmp) {}
    virtual ~Icon() {}

    virtual Rect draw(Pos* pos);

    Bitmap* bmp;
};

#if 0
class Srubber : public Widget
{
    plus/minus vs arrows
        ff arrows
};
#endif

#if 0
class DropDown : public Window
{
};
#endif

#if 0
class ButtonBar;
#endif

#endif

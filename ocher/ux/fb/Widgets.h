#ifndef OCHER_FB_WIDGETS_H
#define OCHER_FB_WIDGETS_H

#include "clc/data/List.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/Event.h"


class Widget : public EventHandler
{
public:
    Widget(int x, int y, unsigned int w, unsigned int h);
    virtual ~Widget();

    void addChild(Widget* child);

    virtual void draw(Pos* pos) = 0;

    int eventReceived(struct OcherEvent* evt);
    virtual int evtKey(struct OcherEvent*) { return -1; }
    virtual int evtMouse(struct OcherEvent*) { return -1; }
    virtual int evtApp(struct OcherEvent*) { return -1; }
    virtual int evtDevice(struct OcherEvent*) { return -1; }

    Rect m_rect;
protected:
    void drawChildren(Pos* pos);

    unsigned int m_flags;
    Widget* m_parent;
    clc::List m_children;
    //focus
    //hidden
};

class Canvas : public Widget
{
public:
    Canvas();
    ~Canvas() {}

    void draw(Pos* pos);

protected:
};

#define OWF_CLOSE 1

class Window : public Widget
{
public:
    Window(int x, int y, unsigned int w, unsigned int h);
    ~Window();

    virtual void draw(Pos* pos);
    virtual void drawBorder(Rect* rect);
    virtual void drawTitle(Rect* rect);
    virtual void drawBg(Rect* rect);
    virtual void drawContent(Rect* rect);

    void setTitle(const char* title);

    uint32_t m_bgColor;
    uint32_t m_borderColor;
    uint8_t m_borderWidth;
    uint32_t m_flags;
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

    virtual void draw(Pos* pos);
    virtual void drawBorder(Rect* rect);
    virtual void drawLabel(Rect* rect);

protected:
    int evtKey(struct OcherEvent*);
    int evtMouse(struct OcherEvent*);

    int border;
    char* m_label;
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

class Icon : public Widget
{
public:
    Icon(int x, int y, unsigned int w, unsigned int h);
    virtual ~Icon() {}

    virtual void draw(Pos*) {}
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

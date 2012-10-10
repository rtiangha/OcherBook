#ifndef OCHER_FB_WIDGETS_H
#define OCHER_FB_WIDGETS_H

#include "clc/data/List.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/Event.h"


extern FrameBuffer* fb;

class Widget : public EventHandler
{
public:
    Widget(int x, int y, unsigned int w, unsigned int h);
    virtual ~Widget();

    void addChild(Widget* child);

    virtual void draw() = 0;

    int eventReceived(struct OcherEvent* evt);
    virtual int evtKey(struct OcherEvent*) { return -1; }
    virtual int evtMouse(struct OcherEvent*) { return -1; }
    virtual int evtApp(struct OcherEvent*) { return -1; }
    virtual int evtDevice(struct OcherEvent*) { return -1; }

protected:
    void drawChildren();

    Rect m_rect;
    Widget* m_parent;
    clc::List m_children;
    //focus
};

class Canvas : public Widget
{
public:
    Canvas();
    ~Canvas() {}

    void draw();

protected:
};

#define OWF_CLOSE 1

class Window : public Widget
{
public:
    Window(int x, int y, unsigned int w, unsigned int h);
    ~Window();

    void draw();
    virtual void draw(int) {}

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
    void draw();

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

class Spinner : public Widget
{
    state;
    speed;
};

class TextEntry : public Window
{
};

class OSKeyboard : public Widget
{
};

class Label : public Widget
{
    label;
    font;
    size;
    attrs;
};

class Icon : public Widget
{
};

class Srubber : public Widget
{
    plus/minus vs arrows
        ff arrows
};

class DropDown : public Window
{
};

class ButtonBar;

#endif

#endif


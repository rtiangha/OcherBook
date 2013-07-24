#ifndef OCHER_UX_READACTIVITY_H
#define OCHER_UX_READACTIVITY_H

#include "ocher/ux/Activity.h"


class Renderer;
class Layout;

class ReadActivity : public Window
{
public:
    ReadActivity(Controller* c);

protected:
    int evtKey(struct OcherKeyEvent*);
    int evtMouse(struct OcherMouseEvent*);

    void onAttached();
    void onDetached();

    void draw();

    Controller* m_controller;

    Layout* m_layout;
    Renderer* renderer;
    Meta* meta;
    unsigned int m_pageNum;
    int atEnd;
    unsigned int m_pagesSinceRefresh;
};

#endif


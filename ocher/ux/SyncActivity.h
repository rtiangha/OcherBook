#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ocher/ux/Activity.h"


class SyncActivity : public Panel
{
public:
    SyncActivity(Controller* c);

    Rect draw(Pos* pos);

protected:
    void onAttached();
    void onDetached();

    Controller* m_controller;

    void processFiles(const char** files);
    void processFile(const char* file);
};

#endif


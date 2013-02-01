#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ocher/ux/Activity.h"


class SyncActivity
{
public:
    SyncActivity(Controller* c);

    Activity run();

protected:
    Controller* m_controller;

    void processFiles(const char** files);
    void processFile(const char* file);
};

#endif


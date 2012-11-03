#ifndef OCHER_UX_SYNCACTIVITY_H
#define OCHER_UX_SYNCACTIVITY_H

#include "ocher/ux/Activity.h"


class SyncActivity
{
public:
    SyncActivity();

    Activity run();

protected:
    void processFiles(const char** files);
    void processFile(const char* file);
};

#endif


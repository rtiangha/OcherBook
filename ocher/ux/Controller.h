#ifndef OCHER_UX_CONTROLLER_H
#define OCHER_UX_CONTROLLER_H

#include "ocher/ux/HomeActivity.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/ReadActivity.h"
#include "ocher/ux/SettingsActivity.h"
#include "ocher/ux/SyncActivity.h"


class Controller
{
public:
    Controller();

    void run();

    UiBits ui;

protected:
    HomeActivity m_homeActivity;
    LibraryActivity m_libraryActivity;
    ReadActivity m_readActivity;
    SettingsActivity m_settingsActivity;
    SyncActivity m_syncActivity;
};

#endif


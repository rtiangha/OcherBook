#ifndef OCHER_CONTROLLER_H
#define OCHER_CONTROLLER_H

#include "ocher/device/Battery.h"
#include "ocher/ux/fb/BatteryIcon.h"
#include "ocher/ux/SyncActivity.h"
#include "ocher/ux/HomeActivity.h"
#include "ocher/ux/ReadActivity.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/SettingsActivity.h"
#include "ocher/ux/fb/SystemBar.h"


class Controller
{
public:
    Controller();

    void processFile(const char* file);
    void processFiles(const char** files);

    void run();

    Battery m_battery;

    SystemBar m_systemBar;

    // TODO: move to shelf object?
    clc::List m_meta;

protected:

    SyncActivity m_syncActivity;
    HomeActivity m_homeActivity;
    ReadActivity m_readActivity;
    LibraryActivity m_libraryActivity;
    SettingsActivity m_settingsActivity;
};

#endif


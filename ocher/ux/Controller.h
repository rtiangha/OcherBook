#ifndef OCHER_UX_CONTROLLER_H
#define OCHER_UX_CONTROLLER_H

#include "ocher/ux/HomeActivity.h"
#include "ocher/ux/LibraryActivity.h"
#include "ocher/ux/ReadActivity.h"
#include "ocher/ux/SettingsActivity.h"
#include "ocher/ux/SyncActivity.h"

#include "ocher/shelf/Shelf.h"


class Context
{
public:
    Context() : shortList(&library), selected(0) {}
    Library library;
    ShortList shortList;

    Meta* selected;
};

class Controller
{
public:
    Controller();

    void run();

    Context ctx;
    UiBits ui;

protected:
    HomeActivity m_homeActivity;
    LibraryActivity m_libraryActivity;
    ReadActivity m_readActivity;
    SettingsActivity m_settingsActivity;
    SyncActivity m_syncActivity;
};

#endif


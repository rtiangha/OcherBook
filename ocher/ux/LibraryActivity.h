#ifndef OCHER_UX_LIBRARYACTIVITY_H
#define OCHER_UX_LIBRARYACTIVITY_H

#include "ocher/ux/Activity.h"
#include "clc/data/List.h"

class Meta;

class LibraryActivity
{
public:
    LibraryActivity(Controller* c) : m_controller(c) {}
    ~LibraryActivity() {}

    Activity run();

protected:
    Controller* m_controller;
};


#endif


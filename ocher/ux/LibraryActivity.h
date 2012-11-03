#ifndef OCHER_UX_LIBRARYACTIVITY_H
#define OCHER_UX_LIBRARYACTIVITY_H

#include "ocher/ux/Activity.h"
#include "clc/data/List.h"

class Meta;

class LibraryActivity
{
public:
    LibraryActivity() {}
    ~LibraryActivity() {}

    Activity run();

//    virtual bool init() { return true; }
//    // TODO:  instead return some epub meta record and/or requested action
//    virtual Meta* browse(clc::List* meta) = 0;
//    virtual void read(Meta* meta) = 0;
};


#endif


#ifndef OCHER_UX_BROWSE_H
#define OCHER_UX_BROWSE_H

#include "clc/data/List.h"

class UiFactory;
class Meta;

class Browse
{
public:
    Browse() {}
    virtual ~Browse() {}

    virtual bool init() { return true; }
    // TODO:  instead return some epub meta record and/or requested action
    virtual Meta* browse(clc::List& meta) = 0;
    virtual void read(UiFactory* factory, Meta* meta) = 0;
};


#endif


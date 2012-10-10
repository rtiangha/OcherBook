#ifndef OCHER_UX_FB_BROWSE_H
#define OCHER_UX_FB_BROWSE_H

#include "ocher/ux/Browse.h"


class BrowseFb : public Browse
{
public:
    BrowseFb();
    ~BrowseFb() {}

    bool init();
    Meta* browse(clc::List* meta);
    void read(Meta* meta);
};

#endif



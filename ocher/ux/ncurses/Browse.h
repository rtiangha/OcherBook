#ifndef OCHER_BROWSE_CURSES_H
#define OCHER_BROWSE_CURSES_H

#include <cdk/cdk.h>

#include "clc/data/Buffer.h"

#include "ocher/ux/Browse.h"


class BrowseCurses : public Browse
{
public:
    BrowseCurses();
    ~BrowseCurses() {}

    bool init(CDKSCREEN* screen);
    Meta* browse(clc::List& meta);
    void read(Renderer* renderer, Meta* meta);

protected:
    CDKSCREEN* m_screen;
};


#endif



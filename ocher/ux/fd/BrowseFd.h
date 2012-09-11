#ifndef OCHER_UX_FD_BROWSE_H
#define OCHER_UX_FD_BROWSE_H

#include "ocher/ux/Browse.h"

class BrowseFd : public Browse
{
public:
    BrowseFd();
    ~BrowseFd() {}

    bool init();
    Meta* browse(clc::List& meta);
    void read(UiFactory* factory, Meta* meta);

protected:
    int m_in;
    int m_out;
};

#endif


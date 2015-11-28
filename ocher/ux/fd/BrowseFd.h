/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FD_BROWSE_H
#define OCHER_UX_FD_BROWSE_H

#include "ocher/ux/Browse.h"

class BrowseFd : public Browse {
public:
    BrowseFd();
    ~BrowseFd()
    {
    }

    bool init();
    Meta *browse(std::vector<Meta *> &meta);
    void read(Meta *meta);

protected:
    int m_in;
    int m_out;
};

#endif

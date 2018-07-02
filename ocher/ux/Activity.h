/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_ACTIVITY_H
#define OCHER_UX_ACTIVITY_H

#include "util/stdex.h"

/** A logical portion of user interaction.
 */
class Activity {
public:
    enum class Type {
        Boot,
        Sleep,
        Sync,
        Home,
        Read,
        Library,
        Settings,
        Previous,
        Quit,
    };
};

#endif

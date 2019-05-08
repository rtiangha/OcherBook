/*
 * Copyright (c) 2018, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_TYPES_H
#define OCHER_UX_TYPES_H

#include <cstdint>

struct Pos {
    int x = 0;
    int y = 0;

    Pos() = default;

    Pos(int _x, int _y) :
        x(_x),
        y(_y)
    {
    }
};

#endif

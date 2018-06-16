/*
 * Copyright (c) 2018, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_TYPES_H
#define OCHER_UX_TYPES_H

#include <cstdint>

struct Pos {
    int16_t x, y;

    Pos()
    {
    }

    Pos(int16_t _x, int16_t _y) :
        x(_x),
        y(_y)
    {
    }
};

#endif

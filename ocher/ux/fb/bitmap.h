/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_RLEIMAGE_H
#define OCHER_UX_FB_RLEIMAGE_H

#include <cstdint>
#include <istream>
#include <vector>

class Bitmap {
public:
    int w = 0;
    int h = 0;
    std::vector<uint8_t> data;
};

/**
 * Run-length encoding.
 * Two consecutive identical bytes indicate a run; the next byte indicates how
 * many additional such bytes follow (0-255).
 */
class RleBitmap {
public:
    friend std::istream& operator>> (std::istream& is, RleBitmap& b);

    static std::vector<uint8_t> pack(const std::vector<uint8_t>& bitmap);
    static std::vector<uint8_t> unpack(const std::vector<uint8_t>& rle);
};

class PgmBitmap {
public:
    friend std::istream& operator>> (std::istream& is, PgmBitmap& b);

    Bitmap bitmap;
};

std::istream& operator>> (std::istream& is, RleBitmap& b);
std::istream& operator>> (std::istream& is, PgmBitmap& b);

#endif

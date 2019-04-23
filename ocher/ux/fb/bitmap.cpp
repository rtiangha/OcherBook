/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/bitmap.h"


std::vector<uint8_t> RleBitmap::pack(const std::vector<uint8_t>& bitmap)
{
    std::vector<uint8_t> rle;
    unsigned int count = 0;
    uint8_t runOf;

    for (unsigned int i = 0; i < bitmap.size(); ++i) {
        if (!count) {
            runOf = bitmap[i];
            count = 1;
        } else if (runOf != bitmap[i] || count == 257) {
            rle.push_back(runOf);
            if (count > 1) {
                rle.push_back(runOf);
                rle.push_back(count - 2);
            }
            runOf = bitmap[i];
            count = 1;
        } else {
            ++count;
        }
    }
    if (count) {
        rle.push_back(runOf);
        if (count > 1) {
            rle.push_back(runOf);
            rle.push_back(count - 2);
        }
    }
    return rle;
}

std::vector<uint8_t> RleBitmap::unpack(const std::vector<uint8_t>& rle)
{
    std::vector<uint8_t> bitmap;
    int runState = 0;
    uint8_t runOf;

    for (unsigned int i = 0; i < rle.size(); ++i) {
        switch (runState) {
        case 2:  // repeat count
            bitmap.insert(bitmap.end(), rle[i], runOf);
            runState = 0;
            break;
        case 1:  // another byte? repeat code required
            if (runOf == rle[i]) {
                bitmap.push_back(runOf);
                runState = 2;
                break;
            }
            //[[fallthrough]]
        case 0:  // new run
            runOf = rle[i];
            bitmap.push_back(runOf);
            runState = 1;
            break;

        };
    }
    return bitmap;
}

std::istream& operator>> (std::istream& is, PgmBitmap& b)
{
    std::string line;
    if (!getline(is, line) || line != "P2" ||
        !getline(is, line)) {
        is.setstate(std::ios::failbit);
        return is;
    }

    is >> b.bitmap.w >> b.bitmap.h;
    int bytes = b.bitmap.w * b.bitmap.h;
    b.bitmap.data.resize(bytes);
    for (int i = 0; i < bytes; ++i) {
        is >> b.bitmap.data[i];
    }
    return is;
}

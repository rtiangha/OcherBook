/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_DEV_BATTERY_H
#define OCHER_DEV_BATTERY_H

/** Reads the battery status.
 */
class Battery {
public:
    Battery();

    enum Status {
        Charging,
        Discharging,
        Unknown
    };

    int readAll();      ///< @return 0 if all read, else -1
    int readCapacity(); ///< @return 0 if read, else -1
    int readStatus();   ///< @return 0 if read, else -1

    int m_percent;      ///< @return 0..100 or -1 if unknown
    Status m_status;
};

#endif

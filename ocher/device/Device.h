/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_DEVICE_H
#define OCHER_DEVICE_H

#include <string>

/** Represents the physical e-reader device.
 */
class Device {
public:
    virtual ~Device()
    {
    }

    std::string getVersion();
    std::string getBuildDate();

    std::string getMac();
    std::string getIp();

    virtual void reboot()
    {
    }

    virtual void shutdown()
    {
    }

    /**
     * Puts the device in a low power state until ??? TODO.
     * Returns when awake.
     */
    virtual void sleep();
};

#endif

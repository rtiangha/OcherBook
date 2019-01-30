/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_DEVICE_H
#define OCHER_DEVICE_H

#include <memory>
#include <string>

/** Represents the physical e-reader device.
 */
class Device {
public:
    static std::unique_ptr<Device> create();

    virtual ~Device() = default;

    virtual const char* make() { return "unknown"; }
    virtual const char* model() { return "unknown"; }
    virtual bool sdCardInserted() { return false; }
    virtual bool sdCardProtected() { return false; }
    virtual void sleep();
    virtual void reboot() {}
    virtual void poweroff() {}
};

#endif

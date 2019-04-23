/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_DEVICE_H
#define OCHER_DEVICE_H

#include <array>
#include <memory>
#include <string>

/** Represents the physical e-reader device.
 */
class Device {
public:
    enum Attr {
        Make,
        Model,
        ModelNumber,
        SerialNumber,
        NumAttrs
    };
    enum ScreenAttr {
        XRes,
        YRes,
        PPI,
        NumScreenAttrs
    };
    enum Prop {
        SDCardInserted,
        SDCardProtected,
        ACPlugIn,
        USBPlugIn,
        NumProps
    };

    std::array<std::string, NumAttrs> attrs;
    std::array<int, NumScreenAttrs> screenattrs;
    std::array<bool, NumProps> props;

    static std::unique_ptr<Device> create();

    virtual ~Device() = default;

    virtual void sleep();
    virtual void reset() {}
    virtual void poweroff() {}
};

#endif

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#ifdef OCHER_TARGET_KOBO
#include <linux/input.h>
#endif

#include "clc/support/Logger.h"

#include "ocher/ocher.h"
#include "ocher/device/Device.h"


Device *device;

void initDevice()
{
    device = new Device();
}

clc::Buffer Device::getMac()
{
    clc::Buffer mac;
    // TODO
    return mac;
}

clc::Buffer Device::getIp()
{
    clc::Buffer ip;
    // TODO
    return ip;
}

clc::Buffer Device::getVersion()
{
    clc::Buffer version;
    version.format("%d.%d.%d", OCHER_MAJOR, OCHER_MINOR, OCHER_PATCH);
    return version;
}

clc::Buffer Device::getBuildDate()
{
    clc::Buffer date(__DATE__);
    return date;
}


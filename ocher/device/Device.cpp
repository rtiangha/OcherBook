/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "device/Device.h"

#include "ocher.h"
#include "util/Logger.h"
#include "util/StrUtil.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev"


std::string Device::getMac()
{
    std::string mac;

    // TODO
    return mac;
}

std::string Device::getIp()
{
    std::string ip;

    // TODO
    return ip;
}

std::string Device::getVersion()
{
    std::string version(format("%d.%d.%d", OCHER_MAJOR, OCHER_MINOR, OCHER_PATCH));

    return version;
}

std::string Device::getBuildDate()
{
    std::string date(__DATE__);

    return date;
}

void Device::sleep()
{
#ifdef __linux__
//    if lsmod | grep -q sdio_wifi_pwr ; then
//        wlarm_le -i eth0 down
//        ifconfig eth0 down
//        /sbin/rmmod -r dhd
//        /sbin/rmmod -r sdio_wifi_pwr
//    fi

// "echo 1 > /sys/power/state-extended"
    const char* pwr = "/sys/power/state";
    int fd = open(pwr, O_WRONLY);
    if (fd == -1) {
        Log::error(LOG_NAME, "%s: %s", pwr, strerror(errno));
    } else {
        write(fd, "mem", 3);
        close(fd);
        ::sleep(1);  // TODO seems hackish, but don't want to return before enters sleep state
    }
#else
    ::sleep(10);  // TODO for testing
#endif
}

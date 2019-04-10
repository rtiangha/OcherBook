/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "device/Device.h"
#ifdef OCHER_TARGET_KOBO
#include "device/ntx.h"
#endif

#include "ocher.h"
#include "util/Logger.h"
#include "util/stdex.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev"

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

std::unique_ptr<Device> Device::create()
{
#ifdef OCHER_TARGET_KOBO
    return make_unique<Kobo>();
#else
    return make_unique<Device>();
#endif
}

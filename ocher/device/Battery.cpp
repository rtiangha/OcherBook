/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "device/Battery.h"

#include "util/Logger.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev.battery"
#define BAT_PATH "/sys/devices/platform/pmic_battery.1/power_supply/mc13892_bat/"


Battery::Battery() :
    percent(-1),
    status(Status::Unknown)
{
    readAll();
}

int Battery::readAll()
{
    int r = 0;

    r |= readCapacity();
    r |= readStatus();
    return r;
}

int Battery::readCapacity()
{
    int r = -1;

#ifdef OCHER_TARGET_KOBO
    int fd = open(BAT_PATH "capacity", O_RDONLY);
    if (fd < 0) {
        Log::error(LOG_NAME, "Failed to read battery capacity: %s", strerror(errno));
    } else {
        char buf[8];
        r = read(fd, buf, sizeof(buf) - 1);
        close(fd);
        if (r > 0) {
            buf[r] = 0;
            percent = atoi(buf);
            Log::info(LOG_NAME, "Battery is %u%%", percent);
            r = 0;
        }
    }
#endif
    if (r == -1)
        percent = -1;
    return r;
}

int Battery::readStatus()
{
#ifdef OCHER_TARGET_KOBO
    int r;
    char buf[5];
    int fd = open(BAT_PATH "status", O_RDONLY);
    if (fd < 0)
        goto unknown;
    r = read(fd, buf, 4);
    close(fd);
    if (r == 4) {
        buf[4] = 0;
        if (strcmp(buf, "Char") == 0) {
            status = Status::Charging;
            return 0;
        } else if (strcmp(buf, "Disc") == 0) {
            status = Status::Discharging;
            return 0;
        }
    }
unknown:
#endif
    status = Status::Unknown;
    return -1;
}

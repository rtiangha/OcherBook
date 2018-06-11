/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "device/Battery.h"

#include "util/Logger.h"
#ifndef DOCTEST_CONFIG_DISABLE
#include "Container.h"
#endif

#include "doctest.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev.battery"
#define BAT_PATH "/sys/devices/platform/pmic_battery.1/power_supply/mc13892_bat/"


Battery::Battery() :
    m_percent(-1),
    m_status(Status::Unknown)
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
        if (r > 0) {
            buf[r] = 0;
            m_percent = atoi(buf);
            Log::info(LOG_NAME, "Battery is %u%%", m_percent);
            r = 0;
        }
        close(fd);
    }
#endif
    if (r == -1)
        m_percent = -1;
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
            m_status = Status::Charging;
            return 0;
        } else if (strcmp(buf, "Disc") == 0) {
            m_status = Status::Discharging;
            return 0;
        }
    }
unknown:
#endif
    m_status = Status::Unknown;
    return -1;
}

TEST_CASE("Battery Capacity") {
    Battery b; // XXX
    int r;

    r = b.readCapacity();
    if (r == -1) {
        CHECK(b.m_percent == -1);
    } else {
        CHECK(b.m_percent >= 0);
        CHECK(b.m_percent <= 100);
    }
}

TEST_CASE("Battery Status") {
    Battery b; // XXX
    int r;

    r = b.readStatus();
    if (r == -1) {
        CHECK(b.m_status == Battery::Status::Unknown);
    } else {
        if (b.m_status != Battery::Status::Charging && b.m_status != Battery::Status::Discharging)
            CHECK(b.m_status == Battery::Status::Charging);
    }
}

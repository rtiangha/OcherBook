/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher_config.h"

#include "settings/Settings.h"

#include "device/Device.h"
#include "device/Filesystem.h"
#include "util/File.h"
#include "util/StrUtil.h"

#include "json.hpp"

#include <cassert>

using nlohmann::json;

const char* Settings::SecureLevelToString(SecureLevel s)
{
    switch (s) {
    case SecureLevel::Open: return "open";
    case SecureLevel::Personal: return "personal";
    case SecureLevel::Private: return "private";
    default: assert(0); return "";
    }
}

const char* Settings::SleepShowToString(SleepShow s)
{
    switch (s) {
    case SleepShow::Sleeping: return "sleeping";
    case SleepShow::Html: return "html";
    case SleepShow::Cover: return "cover";
    case SleepShow::Blank: return "blank";
    default: assert(0); return "";
    }
}

Settings::Settings(Filesystem& filesystem) :
    secureLevel(SecureLevel::Open),
    trackReading(0),
    minutesUntilSleep(15),
    sleepShow(SleepShow::Sleeping),
    sleepHtml("<center>Custom sleep message</center>"),
    minutesUntilPowerOff(60),
    powerOffHtml("<center>Off</center>"),
    wirelessPrompt(0),
    fullRefreshPages(6),
    showPageNumbers(1),
    fontPoints(10),
    marginTop(10),
    marginBottom(10),
    marginLeft(10),
    marginRight(10),
    smallSpace(10),
    medSpace(15),
    largeSpace(30),
    m_filesystem(filesystem),
    m_j(new json)
{
    // TODO  Find these dynamically
#ifdef OCHER_TARGET_KOBO
    fontRoot = "/usr/local/Trolltech/QtEmbedded-4.6.2-arm/lib/fonts";
#elif defined __FreeBSD__
    fontRoot = "/usr/local/share/fonts/";
#else
    fontRoot = "/usr/share/fonts/TTF/";
#endif
}

void Settings::load()
{
    File f;

    if (f.setTo(m_filesystem.m_settings) != 0)
        return;
    std::string s;
    f.readRest(s);
    load(s);
}

void Settings::load(const std::string& data)
{
    // TODO: This seems ugly.  See the "nlohmann::json constructor" test.
    std::stringstream ss(data);
    json j;
    j << ss;

    std::string s;
    bool parsed;

    // For each value, parse if possible (else keep previous), then normalize.

    try {
        s = j["SecureLevel"];
        parsed = true;
    } catch (const std::domain_error&) {
        parsed = false;
    }
    if (parsed) {
        if (s == SecureLevelToString(SecureLevel::Open))
            secureLevel = SecureLevel::Open;
        else if (s == SecureLevelToString(SecureLevel::Personal))
            secureLevel = SecureLevel::Personal;
        else if (s == SecureLevelToString(SecureLevel::Private))
            secureLevel = SecureLevel::Private;
    }

    try {
        trackReading = j["TrackReading"];
    } catch (const std::domain_error&) {
    }
    if (trackReading < 0)
        trackReading = 0;
    else if (trackReading > 2)
        trackReading = 2;

    try {
        minutesUntilSleep = j["MinutesUntilSleep"];
    } catch (const std::domain_error&) {
    }
    if (minutesUntilSleep > 24*60)
        minutesUntilSleep = 24*60;

    try {
        s = j["SleepShow"];
        parsed = true;
    } catch (const std::domain_error&) {
        parsed = false;
    }
    if (parsed) {
        if (s == SleepShowToString(SleepShow::Blank))
            sleepShow = SleepShow::Blank;
        else if (s == SleepShowToString(SleepShow::Sleeping))
            sleepShow = SleepShow::Sleeping;
        else if (s == SleepShowToString(SleepShow::Html))
            sleepShow = SleepShow::Html;
        else if (s == SleepShowToString(SleepShow::Cover))
            sleepShow = SleepShow::Cover;
    }

    try {
        sleepHtml = j["SleepHtml"];
    } catch (const std::domain_error&) {
    }

    try {
        minutesUntilPowerOff = j["MinutesUntilPowerOff"];
    } catch (const std::domain_error&) {
    }
    if (minutesUntilPowerOff > 24*60)
        minutesUntilPowerOff = 24*60;
    if (minutesUntilPowerOff < minutesUntilSleep)
        minutesUntilPowerOff = minutesUntilSleep;

    try {
        sleepHtml = j["PowerOffHtml"];
    } catch (const std::domain_error&) {
    }

    try {
        wirelessSsid = j["WirelessSSID"];
    } catch (const std::domain_error&) {
    }

    try {
        wirelessPrompt = j["WirelessPrompt"];
    } catch (const std::domain_error&) {
    }

    try {
        fullRefreshPages = j["FullRefreshPages"];
    } catch (const std::domain_error&) {
    }

    try {
        showPageNumbers = j["ShowPageNumbers"];
    } catch (const std::domain_error&) {
    }
}

void Settings::save()
{
    json j;

    j["SecureLevel"] = SecureLevelToString(secureLevel);
    j["TrackReading"] = trackReading;
    j["MinutesUntilSleep"] = minutesUntilSleep;
    j["SleepShow"] = SleepShowToString(sleepShow);
    j["SleepHtml"] = sleepHtml;
    j["MinutesUntilPowerOff"] = minutesUntilPowerOff;
    j["PowerOffHtml"] = powerOffHtml;
    j["WirelessSSID"] = wirelessSsid;
    j["WirelessPrompt"] = wirelessPrompt;
    j["FullRefreshPages"] = fullRefreshPages;
    j["ShowPageNumbers"] = showPageNumbers;

    std::string b = j.dump();
    File s(m_filesystem.m_settings, "w");
    s.write(b.c_str(), b.size());
}

/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/device/Device.h"
#include "ocher/device/Filesystem.h"
#include "ocher/settings/Settings.h"
#include "ocher/util/File.h"
#include "ocher/util/StrUtil.h"

#include "json.hpp"

using nlohmann::json;

Settings defaultSettings;

const char *Settings::SecureLevelToString(SecureLevel s)
{
    switch (s) {
    case SecureLevelOpen: return "open";
    case SecureLevelPersonal: return "personal";
    case SecureLevelPrivate: return "private";
    }
}

const char *Settings::SleepShowToString(SleepShow s)
{
    switch (s) {
    case SleepShowSleeping: return "sleeping";
    case SleepShowHtml: return "html";
    case SleepShowCover: return "cover";
    case SleepShowBlank: return "blank";
    }
}

Settings::Settings() :
    secureLevel(SecureLevelOpen),
    trackReading(0),
    minutesUntilSleep(15),
    sleepShow(SleepShowSleeping),
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
    m_fs(NULL),
    m_j(new json)
{
#ifdef OCHER_TARGET_KOBO
    fontRoot = "/usr/local/Trolltech/QtEmbedded-4.6.2-arm/lib/fonts";
#else
    // TODO fontRoot = ".:/usr/share/fonts/truetype/ttf-dejavu";
    // fontRoot = "/usr/share/fonts/truetype/";
    fontRoot = "/usr/local/lib/X11/fonts/";
#endif
}

void Settings::inject(Filesystem *fs)
{
    m_fs = fs;
}

void Settings::load()
{
    File f;

    if (f.setTo(m_fs->m_settings) != 0)
        return;
    std::string s;
    f.readRest(s);
    load(s);
}

void Settings::load(const std::string &data)
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
    } catch (std::domain_error &) {
        parsed = false;
    }
    if (parsed) {
        if (s == SecureLevelToString(SecureLevelOpen))
            secureLevel = SecureLevelOpen;
        else if (s == SecureLevelToString(SecureLevelPersonal))
            secureLevel = SecureLevelPersonal;
        else if (s == SecureLevelToString(SecureLevelPrivate))
            secureLevel = SecureLevelPrivate;
    }

    try {
        trackReading = j["TrackReading"];
    } catch (std::domain_error &) {
    }
    if (trackReading < 0)
        trackReading = 0;
    else if (trackReading > 2)
        trackReading = 2;

    try {
        minutesUntilSleep = j["MinutesUntilSleep"];
    } catch (std::domain_error &) {
    }
    if (minutesUntilSleep > 24*60)
        minutesUntilSleep = 24*60;

    try {
        s = j["SleepShow"];
        parsed = true;
    } catch (std::domain_error &) {
        parsed = false;
    }
    if (parsed) {
        if (s == SleepShowToString(SleepShowBlank))
            sleepShow = SleepShowBlank;
        else if (s == SleepShowToString(SleepShowSleeping))
            sleepShow = SleepShowSleeping;
        else if (s == SleepShowToString(SleepShowHtml))
            sleepShow = SleepShowHtml;
        else if (s == SleepShowToString(SleepShowCover))
            sleepShow = SleepShowCover;
    }

    try {
        sleepHtml = j["SleepHtml"];
    } catch (std::domain_error &) {
    }

    try {
        minutesUntilPowerOff = j["MinutesUntilPowerOff"];
    } catch (std::domain_error &) {
    }
    if (minutesUntilPowerOff > 24*60)
        minutesUntilPowerOff = 24*60;
    if (minutesUntilPowerOff < minutesUntilSleep)
        minutesUntilPowerOff = minutesUntilSleep;

    try {
        sleepHtml = j["PowerOffHtml"];
    } catch (std::domain_error &) {
    }

    try {
        wirelessSsid = j["WirelessSSID"];
    } catch (std::domain_error &) {
    }

    try {
        wirelessPrompt = j["WirelessPrompt"];
    } catch (std::domain_error &) {
    }

    try {
        fullRefreshPages = j["FullRefreshPages"];
    } catch (std::domain_error &) {
    }

    try {
        showPageNumbers = j["ShowPageNumbers"];
    } catch (std::domain_error &) {
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
    File s(m_fs->m_settings, "w");
    s.write(b.c_str(), b.size());
}

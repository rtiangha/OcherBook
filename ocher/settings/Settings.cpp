/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "settings/Settings.h"

#include "device/Device.h"
#include "device/Filesystem.h"
#include "util/File.h"
#include "util/StrUtil.h"

#include "json.hpp"

#include "doctest.h"

#include <cassert>

using nlohmann::json;

Settings defaultSettings;

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

Settings::Settings() :
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
    m_fs(nullptr),
    m_j(new json)
{
    // TODO  Find these dynamically
#ifdef OCHER_TARGET_KOBO
    fontRoot = "/usr/local/Trolltech/QtEmbedded-4.6.2-arm/lib/fonts";
#elif defined __FreeBSD__
    fontRoot = "/usr/local/share/fonts/";
#else
    fontRoot = "/usr/share/fonts/truetype/";
#endif
}

void Settings::inject(Filesystem* fs)
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
    File s(m_fs->m_settings, "w");
    s.write(b.c_str(), b.size());
}

// These tests are to test my understanding of how nlohmann::json works.
// They are also acceptance tests of a new nlohmann::json version.

TEST_CASE("nlohmann::json unparsable") {
    json j;

    std::stringstream ss("}}}{{{ totally unparsable non-json crap:::\"\"\"");
    REQUIRE_THROWS_AS(j << ss, std::invalid_argument);
}

TEST_CASE("nlohmann::json not found") {
    json j;
    int i;

    i = 1;
    j["foo"] = i;

    // clearly, can get an int:
    i = j["foo"];
    (void)i;

    // trying to get an int (but is null) throws
    REQUIRE_THROWS_AS(i = j["bar"], std::domain_error);
}

TEST_CASE("nlohmann::json no type coercion string to int") {
    json j;
    int i;

    // trying to get an int (but is string) throws
    j["foo"] = "foo";
    REQUIRE_THROWS_AS(i = j["foo"], std::domain_error);

    // trying to get an int (but is possibly coerciable string) throws
    j["foo"] = "42";
    REQUIRE_THROWS_AS(i = j["foo"], std::domain_error);

    (void)i;
}

TEST_CASE("nlohmann::json user mistypes int as string") {
    json j;
    int i;

    j["foo"] = "42";
    try {
        i = j["foo"];
        CHECK(false);  // did throw
    } catch (std::domain_error&) {
        std::string s = j["foo"];
        i = atoi(s.c_str());
        CHECK(i == 42);
    }
}

TEST_CASE("nlohmann::json constructor") {
    // Constructor does not work how I expected.  If I pass a json to the constructor, or assign
    // it, accesses throw the exception:
    //    cannot use operator[] with string
#if 0
    // FAILS
    json j("{ \"key\": \"garbage\" }");
#elif 0
    // FAILS
    json j;
    j = "{ \"key\": \"garbage\" }";
#elif 1
    // OK
    json j;
    std::stringstream ss("{ \"key\": \"garbage\" }");
    j << ss;
#else
    // OK
    json j;
    j["key"] = "garbage";
#endif

    std::string v;

    v = j["key"];
    CHECK(v == "garbage");

    v = j.value("key", "default-value");
    CHECK(v == "garbage");

    v = j.value("key", std::string("default-value"));
    CHECK(v == "garbage");

    std::string key("key");
    std::string defaultValue("default-value");
    v = j[key];
    CHECK(v == "garbage");
}

TEST_CASE("SecureLevel") {
    Settings s;

    // unrecognized is unchanged
    Settings defaultSettings;
    s.load("{ \"SecureLevel\": \"garbage\" }");
    CHECK(s.secureLevel == defaultSettings.secureLevel);
    s.load("{ \"SecureLevel\": 42 }");
    CHECK(s.secureLevel == defaultSettings.secureLevel);

    s.load("{ \"SecureLevel\": \"open\" }");
    CHECK(s.secureLevel == Settings::SecureLevel::Open);

    s.load("{ \"SecureLevel\": \"personal\" }");
    CHECK(s.secureLevel == Settings::SecureLevel::Personal);

    s.load("{ \"SecureLevel\": \"private\" }");
    CHECK(s.secureLevel == Settings::SecureLevel::Private);
}

TEST_CASE("TrackReading") {
    Settings s;

    s.load("{ \"TrackReading\": 0 }");
    CHECK(s.trackReading == 0);

    s.load("{ \"TrackReading\": 1 }");
    CHECK(s.trackReading == 1);

    // unrecognized is unchanged
    s.load("{ \"TrackReading\": \"foo\" }");
    CHECK(s.trackReading == 1);
}

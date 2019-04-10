#include "settings/Settings.h"

#include "device/Filesystem.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

Filesystem g_filesystem;

TEST_CASE("SecureLevel") {
    Settings s(g_filesystem);

    // unrecognized is unchanged
    Settings defaultSettings(g_filesystem);
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
    Settings s(g_filesystem);

    s.load("{ \"TrackReading\": 0 }");
    CHECK(s.trackReading == 0);

    s.load("{ \"TrackReading\": 1 }");
    CHECK(s.trackReading == 1);

    // unrecognized is unchanged
    s.load("{ \"TrackReading\": \"foo\" }");
    CHECK(s.trackReading == 1);
}

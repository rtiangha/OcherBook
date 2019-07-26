/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_SETTINGS_H
#define OCHER_SETTINGS_H

#include <json.hpp>

#include <string>

// TODO: conditionalize some settings based on platform?  so can maintain/share a single settings
//       file
// TODO: per-user settings?
// TODO: per-user histories?  (consider: insert SD card with another user's encrypted history)
// TODO: if secureLevel does not match match reality, must prompt user (do you really want to
//       decrypt?)

class Filesystem;

/** Persisted user settings
 */
class Settings {
public:
    Settings(Filesystem& filesystem);

    enum class SecureLevel {
        Open,
        Personal,
        Private,
    };
    static const char* secureLevelToString(SecureLevel s);

    enum class SleepShow {
        Sleeping,
        Html,
        Cover,
        Blank,
    };
    static const char* sleepShowToString(SleepShow s);

    void reset();

    void load();
    void load(const std::string& json);
    void save();

    /** A security level can protect a user's reading history.
     *
     * Example JSON:  `"SecureLevel": "personal"`
     *
     * - `open`: (default) No encryption.  Anyone who picks up the device can see what you can see.
     * - `personal`:  Personal data (metadata such as reading history, notes, and bookmarks) is
     *   encrypted.  The device is normally in your posession, so the password is kept in memory
     *   (until power-off or exhausted battery).
     * - `private`:  Like `personal`, but wake-after-sleep also requires password to access your
     *   metadata.
     */
    SecureLevel secureLevel = SecureLevel::Open;

    /** Track reading events?  Some people like a log of the books they have read.
     *
     * Example JSON:  `"TrackReading": "none"`
     *
     * - `none`: Do not track reading history.
     * - `finish-books`: Only track when a book is finished.
     * - `all-books`: Track whenever a book is read or finished.
     */
    int trackReading = 0;

    /** Minutes of inactivity until sleep.
     *
     * Example JSON:  `"MinutesUntilSleep": 15`
     *
     * - `0`: Never sleep
     */
    unsigned int minutesUntilSleep = 15;

    /** What to show while sleeping.
     *
     * Example JSON:  `"SleepShow": "html"`
     *
     * - `sleeping`: (default) Show the text "Sleeping..."
     * - `html`: Show some custom HTML.
     * - `cover`: Show the cover of the most recent book.
     * - `blank`:  Blank the screen completely.
     */
    SleepShow sleepShow = SleepShow::Sleeping;

    /** Custom HTML to show when sleeping.
     *
     * Example JSON:  `"SleepHtml": "<center>Laura's Kobo</center>"`
     */
    std::string sleepHtml;

    /** Minutes of inactivity until power off.  Lower bounded by minutesUntilSleep.
     *
     * Example JSON:  `"MinutesUntilPowerOff": 1440`
     *
     * - `0`: Never automatically power-off.
     */
    unsigned int minutesUntilPowerOff = 60;

    /**
     */
    std::string powerOffHtml;

    // time zone

    // language

    /**
     */
    std::string wirelessSsid;

    /** Prompt before using wireless?  So-called airplane mode.
     */
    bool wirelessPrompt = false;

    /**
     */
    unsigned int fullRefreshPages = 6;

    /**
     */
    bool showPageNumbers = true;

    int systemFontPoints = 12;
    int fontPoints = 10;

    // force font
    // force font size
    // line spacing
    // justification

    int marginTop = 10;
    int marginBottom = 10;
    int marginLeft = 10;
    int marginRight = 10;

    // icons

    // filesystem point(s)
    std::string fontRoot;

    // UI scaling
    int smallSpace = 10;
    int medSpace = 15;
    int largeSpace = 30;

    // home page arrangement

protected:
    const Filesystem& m_filesystem;

    nlohmann::json m_j;
};

#endif

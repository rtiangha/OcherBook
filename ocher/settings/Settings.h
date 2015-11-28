/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_SETTINGS_H
#define OCHER_SETTINGS_H

#include <string>

// TODO:  conditionalize some settings based on platform?  so can maintain/
// share a single settings files

class Filesystem;

/** Persisted user settings
 */
class Settings {
public:
    Settings();

    void load();
    void save();

    void inject(Filesystem *fs);

    int trackReading;   ///< Track major reading events? (start/end book, ...)
    int encryptReading; ///< Encrypt reading events?

    unsigned int minutesUntilSleep;
    int sleepShowBook;                 ///< When sleeping, show latest book cover?

    unsigned int minutesUntilPowerOff; ///< idle min until poweroff (lower bound by minutesUntilSleep)

    std::string sleepHtml;
    std::string powerOffHtml;

    // time zone

    // language

    std::string wirelessSsid;
    int wirelessAirplaneMode;  ///< Ask before turning on wireless

    unsigned int fullRefreshPages;

    int showPageNumbers;

    int fontPoints;
    // force font
    // force font size
    // line spacing
    int marginTop;
    int marginBottom;
    ;
    int marginLeft;
    int marginRight;
    // justification

    // icons

    // filesystem point(s)
    std::string fontRoot;

    // UI scaling
    int smallSpace;
    int medSpace;
    int largeSpace;

    // home page arrangement

protected:
    Filesystem *m_fs;
};

#endif

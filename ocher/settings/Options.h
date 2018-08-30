/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_OPTIONS_H
#define OCHER_OPTIONS_H

#include <map>
#include <string>
#include <vector>

/** Options set per program run
 */
class Options {
public:
    int verbose = 0;

    /** Start at boot menu?
     */
    bool bootMenu = false;

    bool listDrivers = false;

    const char* driverName = nullptr;

    /** Files and/or directories passed on the command line, to be synced once.
     */
    std::vector<std::string> files;

    int inFd = 0;
    int outFd = 1;

    std::map<std::string, std::string> keys;

    /** User's password, to unlock a secure level.  Typically cleared on power-off or sleep.
     */
    std::string password;
};

#endif

/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_OPTIONS_H
#define OCHER_OPTIONS_H

#include <map>
#include <string>

/** Options set per program run
 */
class Options {
public:
    Options() :
        verbose(0),
        bootMenu(false),
        driverName(0),
        files(0),
        watches(0),
        inFd(0),
        outFd(1)
    {
    }

    int verbose;

    /** Start at boot menu?
     */
    bool bootMenu;

    const char *driverName;

    /** Files and/or directories passed on the command line, to be synced once.
     */
    const char **files;

    /** @todo directories to watch
     */
    const char **watches;

    int inFd;
    int outFd;

    std::map<std::string, std::string> keys;

    /** User's password, to unlock a secure level.  Typically cleared on power-off or sleep.
     */
    std::string password;
};

#endif

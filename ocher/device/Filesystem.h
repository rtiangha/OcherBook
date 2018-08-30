/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_FILESYSTEM_H
#define OCHER_FILESYSTEM_H

#include "ux/Event.h"

#include "Signal.h"
using namespace Gallant;


/** Represents the interesting points of the filesystem (namely, where the books exist, and where
 * user settings might be stored.)
 *
 * On some platforms, notifies when the books change.
 */
class Filesystem {
public:
    Filesystem();
    ~Filesystem();

    Filesystem(const Filesystem&) = delete;
    Filesystem& operator=(const Filesystem&) = delete;

    std::vector<std::string> m_libraries;
    std::string m_home;
    std::string m_settings;

    void initWatches(EventLoop& loop);
    void deinitWatches(EventLoop& loop);

    /**
     * @param name  The file or directory with the event
     * @param what  Description of the event
     */
    Signal2<const char*, const char*> dirChanged;

protected:
#ifdef __linux__
    int m_notifyFd = -1;
    struct ev_io m_watcher;
    static void _watchCb(struct ev_loop* loop, ev_io* watcher, int revents);
    void watchCb();
#endif
};

#endif

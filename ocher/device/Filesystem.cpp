/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "device/Filesystem.h"

#include "settings/Options.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/Path.h"

#include <cstdlib>
#include <sys/stat.h>
#if !defined(__HAIKU__)
#include <pwd.h>
#include <unistd.h>
#endif
#ifdef __linux__
#include <sys/inotify.h>
#endif

#define LOG_NAME "ocher.dev.fs"

#ifndef OCHER_TARGET_KOBO
static std::string settingsDir()
{
    std::string dir;

#if defined(__HAIKU__)
    dir = "/boot/home/config/settings";
#else
    const char* e = getenv("HOME");
    if (!e) {
        struct passwd* p = getpwuid(getuid());
        if (p) {
            e = p->pw_dir;
        } else {
            e = "/tmp";
        }
    }
    dir = e;
#if defined(__APPLE__)
    dir = Path::join(dir, "Library");
    dir = Path::join(dir, "Application Support");
#endif
#endif
    return dir;
}
#endif

Filesystem::Filesystem() :
    m_libraries(nullptr),
    m_infd(-1)
{
#ifdef OCHER_TARGET_KOBO
    m_libraries = new const char*[3];
    m_libraries[0] = "/mnt/sd";
    m_libraries[1] = "/mnt/onboard";
    m_libraries[2] = 0;
    m_home = "/mnt/onboard/.ocher";
    m_settings = "/mnt/onboard/.ocher/settings";
#else
    std::string s = settingsDir();
#if defined(__HAIKU__) || defined(__APPLE__)
    s = Path::join(s, "OcherBook");
#else
    s = Path::join(s, ".OcherBook");
#endif
    m_home = strdup(s.c_str());
    ::mkdir(s.c_str(), 0775);
    s = Path::join(s, "settings");
    m_settings = strdup(s.c_str());
#endif
}

Filesystem::~Filesystem()
{
#ifndef OCHER_TARGET_KOBO
    free(m_home);
    free(m_settings);
#endif
    delete m_libraries;
}

void Filesystem::initWatches(Options* options)
{
#ifdef __linux__
    m_infd = inotify_init();
    if (m_infd == -1) {
        Log::error(LOG_NAME, "inotify_init: %s", strerror(errno));
        return;
    }

    if (!options->files)
        return;
    for (int i = 0;; ++i) {
        const char* lib = options->files[i]; // TODO  m_libraries[i];
        if (!lib)
            break;
        int wd = inotify_add_watch(m_infd, lib, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
        if (wd == -1) {
            Log::error(LOG_NAME, "inotify_add_watch(\"%s\"): %s", lib, strerror(errno));
        }
    }
#endif
}

void Filesystem::deinitWatches()
{
#ifdef __linux__
    close(m_infd);
#endif
}

void Filesystem::fireEvents()
{
#ifdef __linux__
    char buff[4096];
    ssize_t len = read(m_infd, buff, sizeof(buff));
    ssize_t i = 0;
    while (i < len) {
        struct inotify_event* pevent = reinterpret_cast<struct inotify_event*>(&buff[i]);

        // if (pevent->len)
        //  pevent->name);

        if (pevent->mask & IN_ACCESS) {
        }      // " was read"
        if (pevent->mask & IN_ATTRIB) {
        }      // " Metadata changed"
        if (pevent->mask & IN_CLOSE_WRITE) {
        }      // " opened for writing was closed"
        if (pevent->mask & IN_CLOSE_NOWRITE) {
        }      // " not opened for writing was closed"
        if (pevent->mask & IN_CREATE) {
        }      // " created in watched directory"
        if (pevent->mask & IN_DELETE) {
        }      // " deleted from watched directory"
        if (pevent->mask & IN_DELETE_SELF) {
        }      // "Watched file/directory was itself deleted"
        if (pevent->mask & IN_MODIFY) {
        }      // " was modified"
        if (pevent->mask & IN_MOVE_SELF) {
        }      // "Watched file/directory was itself moved"
        if (pevent->mask & IN_MOVED_FROM) {
        }      // " moved out of watched directory"
        if (pevent->mask & IN_MOVED_TO) {
        }      // " moved into watched directory"
        if (pevent->mask & IN_OPEN) {
        }      // " was opened"

        /*
           printf ("wd=%d mask=%d cookie=%d len=%d dir=%s\n",
           pevent->wd, pevent->mask, pevent->cookie, pevent->len,
           (pevent->mask & IN_ISDIR)?"yes":"no");

           if (pevent->len) printf ("name=%s\n", pevent->name);
         */

        dirChanged(pevent->name, "");

        i += sizeof(struct inotify_event) + pevent->len;
    }
#endif
}

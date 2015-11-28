/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/Container.h"
#include "ocher/fmt/Format.h"
#include "ocher/settings/Options.h"
#include "ocher/ux/fb/SyncActivityFb.h"
#include "ocher/ux/fb/UxControllerFb.h"
#include "ocher/util/DirIter.h"
#include "ocher/util/File.h"
#include "ocher/util/Logger.h"
#include "ocher/util/Path.h"

#include <sys/stat.h>
#include <sys/types.h>

#define LOG_NAME "ocher.ux.Sync"


class SyncActivityWork : public EventWork {
public:
    SyncActivityWork(UxControllerFb *controller, const char **files) :
        EventWork(g_container.loop),
        m_files(files),
        m_uxController(controller)
    {
        start();
    }

protected:
    const char **m_files;
    UxControllerFb *m_uxController;
    void work();
    void processFile(const char *file);
    void notify();
};

void SyncActivityWork::work()
{
    Log::info(LOG_NAME "Work", "working");

    for (const char *file = *m_files; file; file = *++m_files) {
        processFile(file);
    }
}

void SyncActivityWork::processFile(const char *file)
{
    struct stat s;

    if (stat(file, &s)) {
        Log::warn(LOG_NAME, "%s: stat: %s", file, strerror(errno));
    } else {
        if (S_ISREG(s.st_mode)) {
            Encoding encoding;
            Fmt format = detectFormat(file, &encoding);
            Log::debug(LOG_NAME, "%s: %s", file, Meta::fmtToStr(format));
            if (format != OCHER_FMT_UNKNOWN) {
                Meta *m = new Meta;
                m->format = format;
                m->relPath = file;
                loadMeta(m);
                m_uxController->ctx.library.add(m);
            }
        } else if (S_ISDIR(s.st_mode)) {
            std::string name;
            DirIter dir(file);
            while (dir.getNext(name) == 0 && name.length()) {
                std::string entryName = Path::join(file, name.c_str());
                processFile(entryName.c_str());
            }
        }
    }
}

void SyncActivityWork::notify()
{
    Log::info(LOG_NAME "Work", "notify");

    m_uxController->ctx.library.notify();
    m_uxController->setNextActivity(ACTIVITY_HOME);
}


void SyncActivityFb::draw()
{
    Log::debug(LOG_NAME, "draw");

    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(&m_rect);
}

SyncActivityFb::SyncActivityFb(UxControllerFb *c) :
    ActivityFb(c),
    m_fb(c->getFrameBuffer()),
    m_spinner(g_container.loop)
{
    maximize();

    int w = m_rect.w / 8;
    m_spinner.setRect(m_rect.w / 2 - w / 2, m_rect.h / 2 - w - 2, w, w);
    addChild(m_spinner);
}

void SyncActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    // TODO:  sync files passed on command line once.  Sync filesystem->m_libraries.  etc.
    m_work = new SyncActivityWork(m_uxController, g_container.options->files);
    g_container.options->files = NULL;
    m_spinner.start();
    invalidate();
}

void SyncActivityFb::onDetached()
{
    Log::info(LOG_NAME, "detached");

    // TODO:  Can be detached not due to work finishing, but being
    // forced out, eg, power saver.  Pause work, don't delete.
    delete m_work;
    m_spinner.stop();
}

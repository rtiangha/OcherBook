/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/fb/SyncActivityFb.h"

#include "Container.h"
#include "fmt/Format.h"
#include "settings/Options.h"
#include "ux/fb/UxControllerFb.h"
#include "util/DirIter.h"
#include "util/File.h"
#include "util/Logger.h"
#include "util/Path.h"

#include <sys/stat.h>
#include <sys/types.h>

#define LOG_NAME "ocher.ux.Sync"


class SyncActivityWork : public EventWork {
public:
    SyncActivityWork(UxControllerFb* controller, const std::vector<std::string>& files) :
        EventWork(g_container->loop),
        m_files(files),
        m_uxController(controller)
    {
        start();
    }

    ~SyncActivityWork() override
    {
        join();
    }

protected:
    std::vector<std::string> m_files;
    UxControllerFb* m_uxController;

    void work() override;
    void notifyComplete() override;

    void processFile(const std::string& file);
};

void SyncActivityWork::work()
{
    Log::info(LOG_NAME "Work", "working");

    for (const auto& file : m_files) {
        processFile(file);
    }

    Log::info(LOG_NAME "Work", "done working");
}

void SyncActivityWork::notifyComplete()
{
    m_uxController->setNextActivity(Activity::Type::Home);
}

void SyncActivityWork::processFile(const std::string& f)
{
    struct stat s;
    const char* file = f.c_str();

    if (stat(file, &s)) {
        Log::warn(LOG_NAME, "%s: stat: %s", file, strerror(errno));
    } else {
        if (S_ISREG(s.st_mode)) {
            Encoding encoding;
            Fmt format = detectFormat(file, &encoding);
            Log::debug(LOG_NAME, "%s: %s", file, Meta::fmtToStr(format));
            if (format != OCHER_FMT_UNKNOWN) {
                auto m = new Meta;
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

void SyncActivityFb::drawContent(const Rect* rect)
{
    Log::debug(LOG_NAME, "draw");

    m_fb->setFg(0xff, 0xff, 0xff);
    m_fb->fillRect(rect);
}

SyncActivityFb::SyncActivityFb(UxControllerFb* c, Filesystem& filesystem) :
    ActivityFb(c),
    m_filesystem(filesystem),
    m_fb(c->getFrameBuffer())
{
    maximize();

    int w = m_rect.w / 8;
    auto spinner = make_unique<Spinner>();
    spinner->setRect(m_rect.w / 2 - w / 2, m_rect.h / 2 - w - 2, w, w);
    m_spinner = spinner.get();
    addChild(std::move(spinner));
}

void SyncActivityFb::onAttached()
{
    Log::info(LOG_NAME, "attached");

    std::vector<std::string> libraries(m_filesystem.m_libraries);
    // Sync files passed on command line only once.
    auto& files = g_container->options.files;
    if (!files.empty()) {
        libraries.insert(libraries.end(), files.begin(), files.end());
        files.clear();
    }
    m_work = new SyncActivityWork(m_uxController, libraries);
    m_spinner->start();
}

void SyncActivityFb::onDetached()
{
    Log::info(LOG_NAME, "detached");

    // TODO:  Can be detached not due to work finishing, but being
    // forced out, eg, power saver.  Pause work, don't delete.
    delete m_work;
    m_spinner->stop();

    m_uxController->ctx.library.notify();
}

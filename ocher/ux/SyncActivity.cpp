#include <sys/types.h>
#include <sys/stat.h>

#include "clc/storage/DirIter.h"
#include "clc/storage/File.h"
#include "clc/storage/Path.h"
#include "clc/support/Logger.h"

#include "ocher/fmt/Format.h"
#include "ocher/settings/Options.h"
#include "ocher/ux/Controller.h"
#include "ocher/ux/Factory.h"
#include "ocher/ux/SyncActivity.h"

#define LOG_NAME "ocher.ux.Sync"


class SyncCanvas : public Canvas
{
public:
    SyncCanvas() {}
    ~SyncCanvas() {}

    Rect draw(Pos* pos);
};

Rect SyncCanvas::draw(Pos*)
{
    Rect drawn;
    if (m_flags & WIDGET_DIRTY) {
        clc::Log::info(LOG_NAME, "draw");
        m_flags &= ~WIDGET_DIRTY;
        g_fb->setFg(0xff, 0xff, 0xff);
        g_fb->fillRect(&m_rect);
        drawn = m_rect;
    } else {
        drawn.setInvalid();
    }
    return drawn;
}

void SyncActivity::processFiles(const char** files)
{
    for (const char* file = *files; file; file = *++files) {
        processFile(file);
    }
}

void SyncActivity::processFile(const char* file)
{
    struct stat s;
    if (stat(file, &s)) {
        clc::Log::warn(LOG_NAME, "%s: stat: %s", file, strerror(errno));
    } else {
        if (S_ISREG(s.st_mode)) {
            Encoding encoding;
            Fmt format = detectFormat(file, &encoding);
            clc::Log::debug(LOG_NAME, "%s: %s", file, Meta::fmtToStr(format));
            if (format != OCHER_FMT_UNKNOWN) {
                Meta* m = new Meta;
                m->format = format;
                m->relPath = file;
                // TODO: loadMeta
                m->title = file; //TODO
                m->author = "Some Author";  //TODO
                g_shelf->add(m);
            }
        } else if (S_ISDIR(s.st_mode)) {
            clc::Buffer name;
            clc::DirIter dir(file);
            while (dir.getNext(name) == 0 && name.length()) {
                clc::Buffer entryName = clc::Path::join(file, name);
                processFile(entryName.c_str());
            }
        }
    }
}

SyncActivity::SyncActivity()
{
}

Activity SyncActivity::run()
{
    clc::Log::info(LOG_NAME, "run");
    SyncCanvas c;
    c.refresh();

    processFiles(opt.files);

    return ACTIVITY_HOME;
}

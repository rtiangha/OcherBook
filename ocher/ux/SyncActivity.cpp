#include <sys/types.h>
#include <sys/stat.h>

#include "clc/storage/File.h"
#include "clc/storage/Path.h"
#include "clc/storage/DirIter.h"
#include "clc/support/Logger.h"
#include "ocher/ux/Factory.h"
#include "ocher/fmt/Format.h"
#include "ocher/settings/Options.h"
#include "ocher/ux/SyncActivity.h"
#include "ocher/ux/Controller.h"



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
        clc::Log::warn("ocher", "%s: stat: %s", file, strerror(errno));
    } else {
        if (S_ISREG(s.st_mode)) {
            Fmt format = detectFormat(file);
            clc::Log::info("ocher", "%s: %s", file, Meta::fmtToStr(format));
            if (format != OCHER_FMT_UNKNOWN) {
                Meta* m = new Meta;
                m->format = format;
                m->relPath = file;
                // TODO: loadMeta
                m->title = file; //TODO
                m->author = "Some Author";  //TODO
                g_cont->m_meta.add(m);
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
    OcherCanvas c;
    c.draw();
    g_fb->update(&c.m_rect);

    processFiles(opt.files);

    return ACTIVITY_HOME;
}

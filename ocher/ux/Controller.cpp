#include <sys/types.h>
#include <sys/stat.h>

#include "mxml.h"

#include "clc/storage/File.h"
#include "clc/storage/Path.h"
#include "clc/storage/DirIter.h"
#include "clc/support/Logger.h"

#include "ocher/ux/Factory.h"
#include "ocher/ux/Controller.h"
#include "ocher/settings/Options.h"

#include "ocher/fmt/epub/Epub.h"
#include "ocher/fmt/epub/LayoutEpub.h"
#include "ocher/fmt/text/Text.h"
#include "ocher/fmt/text/LayoutText.h"
#include "ocher/shelf/Meta.h"


Controller::Controller(UiFactory *factory) :
    m_factory(factory)
{
    // TODO:  kick off event loop; give it Controller
}

void Controller::processFiles(const char** files)
{
    for (const char* file = *files; file; file = *++files) {
        processFile(file);
    }
}

void Controller::processFile(const char* file)
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
                m_meta.add(m);
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

void Controller::run()
{
    processFiles(opt.files);

    while (1) {
        Meta* meta = 0;
        Browse* browser = m_factory->getBrowser();

        // TODO:  home screen

        meta = browser->browse(m_meta);
        if (!meta)
            break;

        clc::Buffer memLayout;
        // TODO:  rework Layout constructors to have separate init due to scoping
        Layout *layout = 0;
        const char* file = meta->relPath.c_str();
        clc::Log::info("ocher", "Loading %s: %s", Meta::fmtToStr(meta->format), file);
        switch (meta->format) {
            case OCHER_FMT_TEXT: {
                Text text(file);
                layout = new LayoutText(&text);
                memLayout = layout->unlock();
                break;
            }
            case OCHER_FMT_EPUB: {
                Epub epub(file);
                layout = new LayoutEpub(&epub);
                clc::Buffer html;
                for (int i = 0; ; i++) {
                    if (epub.getSpineItemByIndex(i, html) != 0)
                        break;
                    mxml_node_t *tree = epub.parseXml(html);
                    if (tree) {
                        ((LayoutEpub*)layout)->append(tree);
                        mxmlDelete(tree);
                    } else {
                        clc::Log::warn("ocher", "No tree found for spine item %d", i);
                    }
                }
                memLayout = layout->unlock();
            }
            default: {
                clc::Log::warn("ocher", "Unhandled format %d", meta->format);
            }
        }

        Renderer* renderer = m_factory->getRenderer();
        renderer->set(memLayout);

        // Optionally, run through all pages without blitting to get an accurate
        // page count.
#if 1
        for (int pageNum = 0; ; pageNum++) {
            clc::Log::info("ocher", "Paginating page %d", pageNum);
            int r = renderer->render(&meta->m_pagination, pageNum, false);

            if (r != 0)
                break;
        }
#endif

        browser->read(renderer, meta);

        delete layout;
    }
}


#include "util/LogAppenders.h"
#include "util/Logger.h"
#include "ux/fb/UxControllerFb.h"
#include "ux/fb/Widgets.h"
#include "Container.h"

#include <getopt.h>

#define LOG_NAME "fbctl"

static Logger* s_log;

void usage(const char* msg = nullptr)
{
    if (msg != nullptr) {
        printf("%s\n\n", msg);
    }
    printf(
        "Usage: fbctl [OPTIONS]...\n"
        "\n"
        "Direct framebuffer:\n"
        "-b,--bg N     Sets the background color (0-255, default 255)\n"
        "-f,--fb N     Sets the foreground color (0-255, default 0)\n"
        "-o,--outline  Outline the screen\n"
        "-c,--clear    Clears the screen with the background color\n"
        "-u,--update   Updates the screen\n"
        "-s,--sync     Syncs the screen (waits for prior update to complete)\n"
        // TODO "-r,--rotate\n"
        "\n"
        "Through window system:\n"
        "-t,--touch    Show touches\n"
        "\n"
        "Misc:\n"
        "-v,--verbose  Increase logging verbosity\n"
        "-q,--quiet    Quiet; decrease logging verbosity\n"
        "-h,--help     Help\n");
    exit(msg != nullptr ? 0 : 1);
}

void setLogLevel(int level)
{
    auto root = Log::get("");
    if (level < 0)
        root->setLevel(Log::Fatal);
    else if (level == 0)
        root->setLevel(Log::Error);
    else if (level == 1)
        root->setLevel(Log::Warn);
    else if (level == 2)
        root->setLevel(Log::Info);
    else if (level == 3)
        root->setLevel(Log::Debug);
    else
        root->setLevel(Log::Trace);
}

void stopLoop()
{
    s_log->info("stopping event loop");
    g_container->loop.stop();
}

int main(int argc, char** argv)
{
    struct option long_options[] =
    {
        { "clear",   no_argument,       nullptr, 'c' },
        { "bg",      required_argument, nullptr, 'b' },
        { "fg",      required_argument, nullptr, 'f' },
        { "help",    no_argument,       nullptr, 'h' },
        { "sync",    no_argument,       nullptr, 's' },
        { "update",  no_argument,       nullptr, 'u' },
        { "verbose", no_argument,       nullptr, 'v' },
        { "outline", no_argument,       nullptr, 'o' },
        { "quiet",   no_argument,       nullptr, 'q' },
        { "touch",   no_argument,       nullptr, 't' },
        { nullptr,   0,                 nullptr, 0   },
    };

    s_log = Log::get(LOG_NAME);

    Options opt;
    opt.verbose = 3;

    // Controller brings everything up
    Controller controller(opt);
    // Grab FrameBuffer
    auto fb = g_container->uxController->getFrameBuffer();
    if (!fb) {
        s_log->error("Failed to get FrameBuffer");
        return 1;
    }
    // UxControllerFb has its own screen
    FbScreen screen(g_container->loop);
    screen.setFrameBuffer(fb);

    s_log->info("%ux%u %u ppi", fb->xres(), fb->yres(), fb->ppi());

    int bg = 0xff;
    int fg = 0x00;
    while (true) {
        int option_index = 0;
        int ch = getopt_long(argc, argv, "cb:f:hsuvqot", long_options, &option_index);
        if (ch == -1)
            break;
        switch (ch) {
        case 0:
            break;
        case 'v':
            s_log->info("verbose");
            setLogLevel(++opt.verbose);
            break;
        case 'q':
            s_log->info("quiet");
            setLogLevel(--opt.verbose);
            break;
        case 'o': {
            s_log->info("outline");
            int x = fb->xres() - 1;
            int y = fb->yres() - 1;
            fb->line(0, 0, x, 0);
            fb->line(x, 0, x, y);
            fb->line(x, y, 0, y);
            fb->line(0, y, 0, 0);
            fb->line(0, 0, x, y);
            fb->line(0, y, x, 0);
            break;
        }
        case 't': {
            s_log->info("touch");
            auto w = make_unique<Window>();
            w->setTitle("Test Window");
            w->maximize();
            auto button = make_unique<Button>("Press me!");
            button->setPos({200, 200});
            button->pressed.Connect(stopLoop);
            w->addChild(std::move(button));
            screen.addChild(std::move(w));
            g_container->loop.run();
            s_log->info("touch done");
            break;
        }
        case 'c':
            s_log->info("clear");
            fb->clear();
            break;
        case 'b':
            s_log->info("background");
            bg = atoi(optarg);
            bg &= 0xff;
            fb->setBg(bg, bg, bg);
            break;
        case 'f':
            s_log->info("foreground");
            fg = atoi(optarg);
            fg &= 0xff;
            fb->setFg(fg, fg, fg);
            break;
        case 'h':
            s_log->info("help");
            usage();
            break;
        case 'u':
            s_log->info("update");
            fb->update(nullptr, true);
            break;
        case 's':
            s_log->info("sync");
            fb->sync();
            break;
        default:
            usage("Unknown argument");
            break;
        }
    }

    return 0;
}

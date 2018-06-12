#include "util/LogAppenders.h"
#include "util/Logger.h"
#include "ux/fb/mx50/FrameBufferMx50.h"

#include <getopt.h>

#define LOG_NAME "fb"

static LogAppenderCFile logAppender(stderr);

void usage(const char* msg = nullptr)
{
    if (msg != nullptr) {
        printf("%s\n\n", msg);
    }
    printf(
        "Usage: fbctl [OPTIONS]...\n"
        "\n"
        "-c,--clear    Clears the screen with the background color.\n"
        "-b,--bg N     Sets the background color (0-255, default 255)\n"
        "-f,--fb N     Sets the foreground color (0-255, default 0)\n"
        "-s,--sync     Syncs the screen (waits for update to complete).\n"
        "-u,--update   Updates the screen.\n"
        "-h,--help     Help.\n");
    exit(msg != nullptr ? 0 : 1);
}

int main(int argc, char** argv)
{
    struct option long_options[] =
    {
        { "clear",  no_argument,       nullptr, 'c' },
        { "bg",     required_argument, nullptr, 'b' },
        { "fg",     required_argument, nullptr, 'f' },
        { "help",   no_argument,       nullptr, 'h' },
        { "sync",   no_argument,       nullptr, 's' },
        { "update", no_argument,       nullptr, 'u' },
        { nullptr,  0,                 nullptr, 0   },
    };

    auto root = Log::get("");
    root->setAppender(&logAppender);
    root->setLevel(Log::Debug);
    auto log = Log::get(LOG_NAME);

    FrameBufferMx50 fb;
    fb.init();
    log->info("%ux%u %u dpi\n", fb.width(), fb.height(), fb.dpi());

    int bg = 0xff;
    int fg = 0x00;
    while (true) {
        int option_index = 0;
        int ch = getopt_long(argc, argv, "cb:f:hsu", long_options, &option_index);
        if (ch == -1)
            break;
        switch (ch) {
        case 0:
            break;
        case 'c':
            fb.clear();
            break;
        case 'b':
            bg = atoi(optarg);
            fb.setBg(bg, bg, bg);
            break;
        case 'f':
            fg = atoi(optarg);
            fb.setFg(fg, fg, fg);
            break;
        case 'h':
            usage();
            break;
        case 'u':
            fb.update(nullptr);
            break;
        case 's':
            fb.sync();
            break;
        default:
            usage("Unknown argument");
            break;
        }
    }

    return 0;
}

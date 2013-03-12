#include <ev.h>
#include "clc/support/Logger.h"

#include "ocher/ux/Event.h"

#define LOG_NAME "ocher.evt"


EventLoop::EventLoop()
{
    evLoop = EV_DEFAULT;
}

EventLoop::~EventLoop()
{
}

int EventLoop::run()
{
    clc::Log::debug(LOG_NAME, "ev_run");
    ev_run(evLoop, 0);
    clc::Log::debug(LOG_NAME, "ev_run done");

    return 0;
}

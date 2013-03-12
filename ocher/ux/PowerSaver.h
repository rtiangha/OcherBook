#ifndef OCHER_DEV_POWER_SAVER_H
#define OCHER_DEV_POWER_SAVER_H

#include "ocher/ux/Event.h"

class Device;


class PowerSaver
{
public:
    PowerSaver();

    void setEventLoop(EventLoop* loop);
    void setDevice(Device* device);

    void setTimeout(unsigned int seconds);

protected:
    void resetTimeout();

    static void timeoutCb(EV_P_ ev_timer* w, int revents);
    void timeout();

    void onMouseEvent(struct OcherMouseEvent* evt);
    void onKeyEvent(struct OcherKeyEvent* evt);
    void onAppEvent(struct OcherAppEvent* evt);
    void onDeviceEvent(struct OcherDeviceEvent* evt);

    EventLoop* m_loop;
    ev_timer m_timer;
    unsigned int m_seconds;

    Device* m_device;
};

#endif

/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_EVENT_H
#define OCHER_EVENT_H

#include "Signal.h"
#include <ev.h>

#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

using namespace Gallant;


#define OEVT_KEYPRESS           0
#define OEVT_KEY_DOWN           1
#define OEVT_KEY_UP             2
#define OEVTK_ENTER             13
#define OEVTK_UP                273
#define OEVTK_DOWN              274
#define OEVTK_RIGHT             275
#define OEVTK_LEFT              276
#define OEVTK_HOME              278
#define OEVTK_PAGEUP            280
#define OEVTK_PAGEDOWN          281
#define OEVTK_POWER             320

struct OcherKeyEvent {
    unsigned int subtype;
    uint32_t key;
};

#define OEVT_MOUSE1_CLICKED     0
#define OEVT_MOUSE1_2CLICKED    1
#define OEVT_MOUSE1_DOWN        2
#define OEVT_MOUSE1_UP          3
#define OEVT_MOUSE2_CLICKED     4
#define OEVT_MOUSE2_2CLICKED    5
#define OEVT_MOUSE2_DOWN        6
#define OEVT_MOUSE2_UP          7
#define OEVT_MOUSE3_CLICKED     8
#define OEVT_MOUSE3_2CLICKED    9
#define OEVT_MOUSE3_DOWN       10
#define OEVT_MOUSE3_UP         11
#define OEVT_MOUSE_MOTION      12

struct OcherMouseEvent {
    unsigned int subtype;  // up, down, ...
    int buttons;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
};

#define OEVT_DEVICE_SUSPEND     0
#define OEVT_DEVICE_RESUME      1
#define OEVT_DEVICE_POWER       2

struct OcherDeviceEvent {
    unsigned int subtype;
};

#define OEVT_APP_ACTIVATE       0
#define OEVT_APP_DEACTIVATE     1
#define OEVT_APP_CLOSE          2

struct OcherAppEvent {
    unsigned int subtype;
};

#define OEVT_NONE               0
#define OEVT_KEY                1
#define OEVT_MOUSE              2
#define OEVT_APP                3
#define OEVT_DEVICE             4

// TODO: model after sdl more
// TODO: timestamp
struct OcherEvent {
    OcherEvent() :
        type(OEVT_NONE)
    {
    }
    uint8_t type;
    union {
        struct OcherKeyEvent key;
        struct OcherMouseEvent mouse;
        struct OcherDeviceEvent device;
        struct OcherAppEvent app;
    };
};

/** The event loop runs on the main thread.
 */
class EventLoop {
public:
    EventLoop();

    /** Runs the event loop on the current thread.  Returns after stop is called (whether internally
     * or externally).
     *
     * @note  SIGINT may be caught and handled internally.
     *
     * @return 0 if normal termination
     */
    int run();
    void stop();
    /** Defines a new epoch; events timestamped prior to this are silently dropped. */
    void setEpoch();

    /** Injects events to be dispatched as if they had ocurred within the EventLoop.  This allows
     * external event loops to be integrated.  The event will be dispatched as soon as possible.
     */
    void injectEvent(const struct OcherEvent& event);

    /** Connect (@sa Gallant::Signal1::Connect) to be notified of events.  Your event handlers must
     * be fast; ideally non-blocking.
     *
     * Note that, unlike many event dispatching systems, multiple observers may be receiving the
     * events in unspecified order.  The EventLoop does not enforce an ordering or specify a way to
     * exclusively handle the event.
     */
    Signal1<const struct OcherEvent*> emitEvent;

    struct ev_loop* evLoop;

protected:
    uint32_t m_epoch;

    ev_async m_async;
    static void emitInjectedCb(EV_P_ ev_async* w, int revents);
    void emitInjected();

    std::mutex m_lock;
    std::vector<struct OcherEvent> m_events;
};


/** Offloads heavy work from the EventLoop to another thread.  When the work is completed, notifies
 * the EventLoop.  Must be created on the EventLoop's thread.
 */
class EventWork {
public:
    /** Derived class must call start() / join().
     */
    EventWork(EventLoop* loop);
    virtual ~EventWork();

    void start();
    void join();

protected:
    void run();

    /** Do your heavy work here.
     */
    virtual void work() = 0;

    /** Runs on the EventLoop thread.  Useful place to emit signals to let a state machine progress.
     *
     * @todo progress callback
     */
    virtual void notifyComplete()
    {
    }

    static void completeCb(EV_P_ ev_async* w, int revents);
    ev_async m_async;
    EventLoop* m_loop;
    std::thread m_thread;
};

#endif

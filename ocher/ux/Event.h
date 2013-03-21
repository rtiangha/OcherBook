#ifndef OCHER_EVENT_H
#define OCHER_EVENT_H

#include <stdint.h>
#include <ev.h>

#include "Signals/Signal.h"
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

struct OcherKeyEvent
{
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

struct OcherMouseEvent
{
    unsigned int subtype;  // up, down, ...
    int buttons;
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
};

#define OEVT_DEVICE_SUSPEND     0
#define OEVT_DEVICE_RESUME      1
#define OEVT_DEVICE_POWER       2

struct OcherDeviceEvent
{
    unsigned int subtype;
};

#define OEVT_APP_ACTIVATE       0
#define OEVT_APP_DEACTIVATE     1
#define OEVT_APP_CLOSE          2

struct OcherAppEvent
{
    unsigned int subtype;
};

#define OEVT_NONE               0
#define OEVT_KEY                1
#define OEVT_MOUSE              2
#define OEVT_APP                3
#define OEVT_DEVICE             4

// TODO: model after sdl more
// TODO: timestamp
struct OcherEvent
{
    OcherEvent() : type(OEVT_NONE) {}
    uint8_t type;
    union {
        struct OcherKeyEvent key;
        struct OcherMouseEvent mouse;
        struct OcherDeviceEvent device;
        struct OcherAppEvent app;
    };
};

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    int run();
    // flush(timestamp)

    Signal1<struct OcherKeyEvent*> keyEvent;
    Signal1<struct OcherMouseEvent*> mouseEvent;
    Signal1<struct OcherAppEvent*> appEvent;
    Signal1<struct OcherDeviceEvent*> deviceEvent;

    struct ev_loop* evLoop;
};

/**
 * Some other event source (something that does not easily plug into libev).
 * Queues events and signals libev when ready.
 */
class EventSource
{
public:

};

#endif


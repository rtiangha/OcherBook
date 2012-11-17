#ifndef OCHER_EVENT_H
#define OCHER_EVENT_H

#include <stdint.h>


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
    uint16_t x;
    uint16_t y;
    uint16_t pressure;
};

#define OEVT_DEVICE_SUSPEND     0
#define OEVT_DEVICE_RESUME      1
#define OEVT_DEVICE_POWER       2

struct OcherDeviceEvent
{
};

#define OEVT_APP_ACTIVATE       0
#define OEVT_APP_DEACTIVATE     1
#define OEVT_APP_CLOSE          2

struct OcherAppEvent
{
};

#define OEVT_NONE               0
#define OEVT_KEY                1
#define OEVT_MOUSE              2
#define OEVT_APP                3
#define OEVT_DEVICE             4

// TODO: model after sdl more
struct OcherEvent
{
    uint8_t type;
    uint8_t subtype;
    union {
        struct OcherKeyEvent key;
        struct OcherMouseEvent mouse;
        struct OcherDeviceEvent device;
        struct OcherAppEvent app;
    };
};

class EventHandler
{
public:
    virtual int eventReceived(struct OcherEvent* evt) = 0;
};

class EventLoop
{
public:
    virtual ~EventLoop() {}
    /**
     * @return 0 got event, -1 error
     */
    virtual int wait(struct OcherEvent* evt) = 0;
    /**
     * @return -1 handled, -2 pass on, >=0 done
     */
    int run(EventHandler* handler);
};

#endif


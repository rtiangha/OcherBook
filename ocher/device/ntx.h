#ifndef NTX_IO_H
#define NTX_IO_H

#include "device/Device.h"

// Get things

#define CM_SD_IN			117  // 0x75
#define CM_SD_PROTECT			120  // 0x78

#define AC_IN				118  // 0x76
#define CM_USB_Plug_IN			108  // 0x6C
#define CM_USB_AC_STATUS		122  // 0x7A
#define CM_GET_BATTERY_STATUS		206  // 0xCE

#define CM_RTC_WAKEUP_FLAG		123  // 0x7B

#define CM_ROTARY_STATUS		200  // 0xC8
#define CM_GET_KEY_STATUS		201  // 0xC9
#define CM_GET_WHEEL_KEY_STATUS		202  // 0xCA

#define CM_CONTROLLER			121  // 0x79
#define CM_DEVICE_MODULE		129  // 0x81
#define CM_DEVICE_INFO			131  // 0x83

#define CM_GET_UP_VERSION		215  // 0xD7

// Set things

#define CM_AUDIO_PWR			113  // 0x71

#define CM_nLED				101  // 0x65
#define CM_LED_BLINK			127  // 0x7F

#define CM_BLUETOOTH_PWR		126  // 0x7E
#define CM_WIFI_CTRL			208  // 0xD0

// Do things

#define POWER_OFF_COMMAND		192  // 0xC0
#define SYS_RESET_COMMAND		193  // 0xC1
#define CM_SYSTEM_RESET			124  // 0x7C
#define CM_BLUETOOTH_RESET		130  // 0x82
#define POWER_KEEP_COMMAND		205  // 0xCD


class NtxDevice : public Device
{
public:
    static bool probe();

    NtxDevice();
    ~NtxDevice();

    const char* make() override;
    const char* model() override;
    bool sdCardInserted() override;
    bool sdCardProtected() override;
    void sleep() override;
    void reboot() override;
    void poweroff() override;

protected:
    static constexpr const char* dev = "/dev/ntx_io";
    int m_fd;

    void identify();
};

#endif

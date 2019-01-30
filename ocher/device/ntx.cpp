#include "device/ntx.h"

#include "util/Logger.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev.ntx"

struct ebook_device_info {
    char device_name;
    char cpu;
    char controller;
    char wifi;
    char bluetooth;
};

// arch/arm/mach-mx5/mx50_ntx_io.c
// arch/arm/mach-mx5/ntx_hwconfig.c  -- disagrees with the kernel?
// https://libreplanet.org/wiki/Group:Hardware/Freest/e-readers#Kobo
const static struct {
    int pcbID;
    const char* codename;
    const char* model;
    const char* board;
} pcbIds[] =
{
  //{6,  "",         "",               "E60630"},
  //{4,  "",         "",               "E60620"},
  //{2,  "",         "",               "E50600"},
  //{3,  "",         "",               "E60680"},
  //{10, "",         "",               "E606A0"},
  //{5,  "",         "",               "E60670"},
    {1,  "trilogy",  "Touch B",        "E60610"},   // E60610 E60610C E60610D
    {9,  "pixie",    "Mini",           "E50610"},
    {0,  "trilogy",  "Touch C",        "E60610D"},
    {14, "kraken",   "Glo",            "E606B0"},  // E60QB*
    {0,  "phoenix",  "Aura",           "E606F0B"},
    {0,  "dragon",   "Aura HD",        "E606C0"},
    {0,  "dahlia",   "Aura H2O",       "E606G0"},
    {0,  "pika",     "Glo HD",         "E60Q90"},
    {0,  "star",     "Aura Edition 2", "E60QL0"},
    {0,  "snow",     "Touch 2.0",      "E60QM0"},
    {0,  "daylight", "Aura One",       "E70Q00"},
};

//{
//  Netronix_E50600,
//  Netronix_E50610,
//  Netronix_E60610,
//  Netronix_E60610D,
//  Netronix_E60620,
//  Netronix_E60630,
//  Netronix_E60670,
//  Netronix_E60680,
//  Netronix_E606A0,
//  Netronix_E606B0,
//  Netronix_E606C0,
//  Netronix_E606F0B,
//  Netronix_E606G0,
//  Netronix_E60Q90,
//  Netronix_E60QL0,
//  Netronix_E60QM0,
//  Netronix_E70Q00,
//}

bool NtxDevice::probe()
{
    return access(dev, F_OK) == 0;
}

NtxDevice::NtxDevice()
{
    m_fd = open(dev, O_RDWR);
    if (m_fd == -1) {
        Log::error(LOG_NAME, "Failed to open %s: %s", dev, strerror(errno));
    }
}

NtxDevice::~NtxDevice()
{
    close(m_fd);
}

const char* NtxDevice::make()
{
    // TODO
    return "";
}

const char* NtxDevice::model()
{
    // TODO
    return "";
}

bool NtxDevice::sdCardInserted()
{
    unsigned long inserted = 0;
    if (ioctl(m_fd, CM_SD_IN, &inserted) == -1) {
        Log::error(LOG_NAME, "Failed to query SD card inserted: %s", strerror(errno));
    }
    return inserted != 0;
}

bool NtxDevice::sdCardProtected()
{
    unsigned long protect = 0;
    if (ioctl(m_fd, CM_SD_PROTECT, &protect) == -1) {
        Log::error(LOG_NAME, "Failed to query SD card protect: %s", strerror(errno));
    }
    return protect != 0;
}

void NtxDevice::sleep()
{
    // TODO
}

void NtxDevice::reboot()
{
    // TODO
}

void NtxDevice::poweroff()
{
    Log::info(LOG_NAME, "Powering off");
    if (ioctl(m_fd, POWER_OFF_COMMAND) == -1) {
        Log::error(LOG_NAME, "Failed to power off: %s", strerror(errno));
    }
}

void NtxDevice::identify()
{
    struct ebook_device_info info;
    if (ioctl(m_fd, CM_DEVICE_INFO, &info) == -1) {
        Log::error(LOG_NAME, "Failed to query platform: %s", strerror(errno));
    } else {
        Log::info(LOG_NAME, "device_name = %d\n", info.device_name);
        Log::info(LOG_NAME, "cpu         = %d\n", info.cpu        );
        Log::info(LOG_NAME, "controller  = %d\n", info.controller );
        Log::info(LOG_NAME, "wifi        = %d\n", info.wifi       );
        Log::info(LOG_NAME, "bluetooth   = %d\n", info.bluetooth  );
    }
#if 0
    unsigned char platform[32];
    if (ioctl(m_fd, CM_PLATFORM, &platform) == -1) {
        Log::error(LOG_NAME, "Failed to query platform: %s", strerror(errno));
    } else {
        Log::info(LOG_NAME, "Platform is '%s'", platform);
    }
#endif
}

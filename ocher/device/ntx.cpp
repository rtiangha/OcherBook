#include "device/ntx.h"

#include "util/Logger.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LOG_NAME "ocher.dev.ntx"

enum class Kobo::PCB : int8_t {
    Unknown = -1, E60800  = 0 , E60810  =  1, E60820  =  2, E90800  =  3,
    E90810  =  4, E60830  = 5 , E60850  =  6, E50800  =  7, E50810  =  8,
    E60860  =  9, E60MT2  = 10, E60M10  = 11, E60610  = 12, E60M00  = 13,
    E60M30  = 14, E60620  = 15, E60630  = 16, E60640  = 17, E50600  = 18,
    E60680  = 19, E60610C = 20, E60610D = 21, E606A0  = 22, E60670  = 23,
    E606B0  = 24, E50620  = 25, Q70Q00  = 26, E50610  = 27, E606C0  = 28,
    E606D0  = 29, E606E0  = 30, E606F0B = -1, E606G0  = -1, E60Q90  = -1,
    E60QL0  = -1, E60QM0  = -1, E70Q00  = -1, E60K00  = -1,
};

using PCB = Kobo::PCB;

struct HwConfig {
    PCB pcb;
    // TODO more...
};

enum class NtxReq {
    // CM_SD_IN         unsigned long* flag
    SDCardInserted      = 117,
    // CM_SD_PROTECT    unsigned long* flag
    SDCardProtected     = 120,
    // AC_IN            unsigned long* flag
    ACPlugIn            = 118,
    // CM_USB_Plug_IN   unsigned long* flag
    USBPlugIn           = 108,
    // CM_nLED          unsigned long flag
    LEDCmd              = 101,
    // CM_WIFI_CTRL     unsigned long flag
    WiFiCmd             = 208,
    // POWER_OFF_COMMAND
    PowerOffCmd         = 192,
    // SYS_RESET_COMMAND
    SystemResetCmd      = 193,
    // CM_DEVICE_INFO   struct ebook_device_info*
    DeviceInfo          = 131,
};

const static struct KoboIDs {
    PCB pcb;        // from uboot
    int pcbID;      // from ioctl
    const char* codename;
    const char* model;
    const char* modelNumber;
    uint16_t usbID;
    uint16_t yres;
    uint16_t xres;
    float inches;
    uint16_t ppi;
} kobos[] = {
    // TODO fill out unknown/empty values
    { PCB::Unknown, -1, "nickel1",  "eReader",            "N416",  0x0000,  800,  600, 6.0, 170, },
    { PCB::Unknown, -1, "nickel2",  "Wifi",               "N647",  0x0000,  800,  600, 6.0, 170, },
    { PCB::Unknown, -1, "nickel2",  "Wifi",               "N647B", 0x0000,  800,  600, 6.0, 170, },
    { PCB::Unknown, -1, "trilogy",  "Touch",              "N905",  0x4163,  800,  600, 6.0, 170, },
    { PCB::E60610 ,  1, "trilogy",  "Touch B",            "N905B", 0x4163,  800,  600, 6.0, 170, },
    { PCB::E60610D,  1, "trilogy",  "Touch C",            "N905C", 0x4163,  800,  600, 6.0, 170, },
    { PCB::E50610 ,  9, "pixie",    "Mini",               "N705",  0x4183,  800,  600, 5.0, 200, },
    { PCB::E606B0 , 14, "kraken",   "Glo",                "N613",  0x4173, 1024,  768, 6.0, 212, },
    { PCB::E606F0B, -1, "phoenix",  "Aura",               "N514",  0x4203, 1024,  768, 6.0, 212, },
    { PCB::E606C0 , -1, "dragon",   "Aura HD",            "N204B", 0x4193, 1440, 1080, 6.8, 265, },
    { PCB::E606G0 , -1, "dahlia",   "Aura H2O",           "N250",  0x4213, 1440, 1080, 6.8, 265, },
    { PCB::E606G0 , -1, "dahlia",   "Aura H2O Edition 2", "N867",  0x4227, 1440, 1080, 6.8, 265, },
    { PCB::E60Q90 , -1, "pika",     "Glo HD",             "N437",  0x4223, 1448, 1072, 6.0, 300, },
    { PCB::E60QL0 , -1, "star",     "Aura Edition 2",     "N236",  0x4226, 1024,  768, 6.0, 212, },
    { PCB::E60QM0 , -1, "snow",     "Touch 2.0",          "N587",  0x4224,  800,  600, 6.0, 170, },
    { PCB::E70Q00 , -1, "daylight", "Aura One",           "N709",  0x4225, 1872, 1404, 7.8, 300, },
    { PCB::E60K00 , -1, "nova",     "Clara HD",           "N249",  0x0000, 1448, 1072, 6.0, 300, },
    { PCB::Unknown, -1, "frost",    "Forma",              "N782",  0x0000, 1920, 1440, 6.0, 300, },
};

struct DeviceAttr {
    NtxReq request;
    Device::Prop prop;
    const char* description;
} deviceAttrs[] = {
    { NtxReq::SDCardInserted,   Device::SDCardInserted,  "SD card inserted" },
    { NtxReq::SDCardProtected,  Device::SDCardProtected, "SD card write protected" },
    { NtxReq::ACPlugIn,         Device::ACPlugIn,        "AC plugged in" },
    { NtxReq::USBPlugIn,        Device::USBPlugIn,       "USB plugged in" },
};

bool Kobo::probe()
{
    return access(dev, F_OK) == 0;
}

Kobo::Kobo()
{
    m_fd = open(dev, O_RDWR);
    if (m_fd == -1) {
        Log::error(LOG_NAME, "Failed to open %s: %s", dev, strerror(errno));
    }

    identify();
}

Kobo::~Kobo()
{
    close(m_fd);
}


void Kobo::sleep()
{
    // TODO
}

void Kobo::reset()
{
    Log::info(LOG_NAME, "System reset");
    if (ioctl(m_fd, static_cast<long unsigned>(NtxReq::SystemResetCmd)) == -1) {
        Log::error(LOG_NAME, "Failed to reset: %s", strerror(errno));
    }
    std::terminate();
}

void Kobo::poweroff()
{
    Log::info(LOG_NAME, "Powering off");
    if (ioctl(m_fd, static_cast<long unsigned>(NtxReq::PowerOffCmd)) == -1) {
        Log::error(LOG_NAME, "Failed to power off: %s", strerror(errno));
    }
}

void Kobo::led(bool on)
{
    unsigned long arg = on ? 0 : 1;  // XXX backwards?
    if (ioctl(m_fd, static_cast<long unsigned>(NtxReq::LEDCmd), arg) == -1) {
        Log::error(LOG_NAME, "Failed to turn LED %s: %s", on ? "on" : "off", strerror(errno));
    }
    // echo 0 > /sys/class/leds/pmic_ledsg/brightness
}

void Kobo::wifi(bool on)
{
    unsigned long arg = on ? 1 : 0;
    if (ioctl(m_fd, static_cast<long unsigned>(NtxReq::WiFiCmd), arg) == -1) {
        Log::error(LOG_NAME, "Failed to turn wifi %s: %s", on ? "on" : "off", strerror(errno));
    }
}

void Kobo::identify()
{
    attrs[Make] = "Kobo";
    attrs[SerialNumber] = getSerialNumber();
    getPcbID();
    getHwConfig();

    for (const auto& attr : deviceAttrs) {
        unsigned long flag = 0;
        if (ioctl(m_fd, static_cast<long unsigned>(attr.request), &flag) == -1) {
            Log::error(LOG_NAME, "Failed to query %s: %s", attr.description, strerror(errno));
        } else {
            Log::info(LOG_NAME, "%s: %lu", attr.description, flag);
            props[attr.prop] = !!flag;
        }
    }

    setKoboDetails();
}

std::string Kobo::getSerialNumber()
{
    std::string serial;
    int fd = open("/dev/mmcblk0", O_RDONLY);
    if (fd != -1) {
        char buf[48];
        if (pread(fd, buf, 48, 512) == 48) {
            // 00000200  53 4e 2d 4e 39 30 35 43  31 34 38 30 31 31 39 31  |SN-N905C14801191|
            // 00000210  00 5f 72 31 36 30 37 5f  31 31 31 30 31 31 31 33  |._r1607_11101113|
            // 00000220  31 32 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |12..............|
            char* p = buf;
            if (strncmp(p, "SN-", 3) == 0)
            {
                p += 3;
                buf[sizeof(buf)-1] = 0;
                serial = p;
            }
        }
        close(fd);
    }
    return serial;
}

void Kobo::getPcbID()
{
    struct ebook_device_info {
        char pcbID;
        // Rest are worthless; response is hardcoded (and wrong)
        char _cpu;
        char _controller;
        char _wifi;
        char _bluetooth;
    } info;

    if (ioctl(m_fd, static_cast<long unsigned>(NtxReq::DeviceInfo), &info) == -1) {
        Log::error(LOG_NAME, "Failed to query platform: %s", strerror(errno));
    } else {
        pcbID = info.pcbID;
    }
}

void Kobo::getHwConfig()
{
    int fd = open("/dev/mmcblk0", O_RDONLY);
    if (fd != -1) {
        char buf[48];
        if (pread(fd, buf, 48, 0x80000) == 48) {
            auto hwConfig = reinterpret_cast<HwConfig*>(buf);

            pcb = hwConfig->pcb;
        }
        close(fd);
    }
}

void Kobo::setKoboDetails()
{
    const struct KoboIDs* id = nullptr;

    for (const auto& kobo : kobos) {
        if (pcb == kobo.pcb) {
            Log::info(LOG_NAME, "modelNumber '%s' could be: %s (codename %s)",
                    kobo.modelNumber, kobo.model, kobo.codename);
            id = &kobo;
            break;
        }
    }
    if (!id) {
        // Serial number starts with model
        const auto& serial = attrs[SerialNumber];
        for (const auto& kobo : kobos) {
            if (strncmp(serial.c_str(), kobo.modelNumber, strlen(kobo.modelNumber)) == 0) {
                id = &kobo;
                break;
            }
        }
    }
    if (!id) {
        // PCB ID obtained via ioctl is remapped from uboot and probably the
        // least reliable.
        for (const auto& kobo : kobos) {
            if (pcbID == kobo.pcbID) {
                Log::info(LOG_NAME, "pcbID %d could be: %s (codename %s)",
                        pcbID, kobo.model, kobo.codename);
                id = &kobo;
                break;
            }
        }
    }

    if (!id) {
        // TODO
    }

    attrs[Model]       = id->model;
    attrs[ModelNumber] = id->modelNumber;
    screenattrs[XRes]  = id->xres;
    screenattrs[YRes]  = id->yres;
    screenattrs[PPI]   = id->ppi;
}

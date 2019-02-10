#ifndef OCHER_DEVICE_KOBO_H
#define OCHER_DEVICE_KOBO_H

#include "device/Device.h"

class Kobo : public Device
{
public:
    enum class PCB : int8_t;

    static bool probe();

    Kobo();
    ~Kobo();

    void sleep() override;
    void reset() override;
    void poweroff() override;

    void led(bool on);
    void wifi(bool on);

protected:
    static constexpr const char* dev = "/dev/ntx_io";
    int m_fd;

    void identify();

    std::string getSerialNumber();
    void getPcbID();
    void getHwConfig();

    void setKoboDetails();

    PCB pcb;        // from uboot
    int pcbID;      // from ioctl
};

#endif

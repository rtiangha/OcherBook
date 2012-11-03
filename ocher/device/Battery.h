#ifndef OCHER_KOBO_BATTERY_H
#define OCHER_KOBO_BATTERY_H

class Battery
{
public:
    Battery();

    enum Status {
        Charging,
        Discharging,
        Unknown
    };

    int readAll();
    virtual int readCapacity(); // { m_percent = 0; return 0; }
    virtual int readStatus(); // { m_status = Unknown; return 0; }

    int m_percent;  // 0..100 or -1 if unknown
    Status m_status;
};

#endif


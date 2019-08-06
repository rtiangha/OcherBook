#include "util/time.h"

#include <chrono>
#include <ctime>

struct tm localTime()
{
    const auto now = std::chrono::system_clock::now();
    const auto tt = std::chrono::system_clock::to_time_t(now);
    struct tm currentLocalTime;
    localtime_r(&tt, &currentLocalTime);
    return currentLocalTime;
}

float secondsUntilMinute(const struct tm& now)
{
    float sec = 60.0 - now.tm_sec;
    // beware leap seconds
    if (sec <= 0.0)
        sec = 1.0;
    return sec;
}

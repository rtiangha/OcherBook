#ifndef OCHER_TIME_H
#define OCHER_TIME_H

#include <ctime>

struct tm localTime();
float secondsUntilMinute(const struct tm& now);

#endif

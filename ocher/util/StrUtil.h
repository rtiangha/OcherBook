#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdarg.h>
#include <string>

std::string formatList(const char *fmt, va_list argList);

std::string format(const char *fmt, ...);

std::string &appendFormatList(std::string &s, const char *fmt, va_list argList);

std::string &appendFormat(std::string &s, const char *fmt, ...);

#endif

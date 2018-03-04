#ifndef OCHER_STRING_UTILS_H
#define OCHER_STRING_UTILS_H

#include <cstdarg>
#include <string>

std::string formatList(const char* fmt, va_list argList);

std::string format(const char* fmt, ...);

std::string& appendFormatList(std::string& s, const char* fmt, va_list argList);

std::string& appendFormat(std::string& s, const char* fmt, ...);

#endif

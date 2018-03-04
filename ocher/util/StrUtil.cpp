#include "util/StrUtil.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>


namespace str {

#if 0
Buffer hexToBin(const Buffer& hex)
{
    Buffer bin;
    size_t bytes = hex.length();

    bytes >>= 1;
    const char *_hex = hex.c_str();
    char *p = bin.lockBuffer(bytes);
    for (size_t i = 0; i < bytes; ++i) {
        p[i] = hexStringToByte(_hex + i * 2);
    }
    bin.unlockBuffer(bytes);
    return bin;
}

Buffer binToHex(const Buffer& bin)
{
    Buffer hex;
    size_t bytes = bin.length();
    char *p = hex.lockBuffer(bytes * 2);

    for (size_t i = 0; i < bytes; ++i) {
        hexByteToString(bin.ByteAt(i), p + i * 2);
    }
    hex.unlockBuffer(bytes * 2);
    return hex;
}

char hexByteToChar(const char byte)
{
    return ((byte & 0x0f) > 9) ? ((byte & 0x0f) - 10 + 'a') : ((byte & 0x0f) + '0');
}

void hexByteToString(const char byte, char *hex)
{
    hex[0] = hexByteToChar(byte >> 4);
    hex[1] = hexByteToChar(byte);
}

void hexBytesToString(const unsigned char *buffer, size_t buflen, char *hex)
{
    while (buflen--) {
        hexByteToString(*buffer++, hex);
        hex += 2;
    }
    *hex = '\0';
}

unsigned char hexStringToByte(const char hex[2])
{
    char rval = hexCharToByte(hex[0]);

    rval <<= 4;
    rval += hexCharToByte(hex[1]);
    return rval;
}

void hexStringToBytes(const char *hex, size_t bufferlen, unsigned char *buffer)
{
    while (bufferlen--) {
        *buffer++ = hexStringToByte(hex);
        hex += 2;
    }
}

std::ostream& operator<<(std::ostream& out, const Buffer& s)
{
    for (size_t i = 0; i < s.length(); ++i) {
        out << s[i];
    }
    return out;
}

Buffer format(const char *fmt, ...)
{
    char *buf;
    va_list argList;

    va_start(argList, fmt);
    int len = vasprintf(&buf, fmt, argList);
    va_end(argList);
    Buffer s;
    if (len >= 0) {
        s.setTo(buf, len);
        free(buf);
    }
    return s;
}

Buffer format(const char *fmt, va_list argList)
{
    char *buf;
    int len = vasprintf(&buf, fmt, argList);
    Buffer s;

    if (len >= 0) {
        s.setTo(buf, len);
        free(buf);
    }
    return s;
}

void appendFormat(Buffer& s, const char *fmt, ...)
{
    va_list argList;

    va_start(argList, fmt);
    appendFormat(s, fmt, argList);
    va_end(argList);
}

void appendFormat(Buffer& s, const char *fmt, va_list argList)
{
    char *buf;
    int len = vasprintf(&buf, fmt, argList);

    if (len >= 0) {
        s.append(buf, len);
        free(buf);
    }
}

uint64_t toUInt(const Buffer& s, bool *valid)
{
    uint64_t v = 0;
    size_t i = 0;

    for (; i < s.length(); ++i) {
        char c = s[i];
        if (c >= '0' && c <= '9')
            v = (v * 10) + (c - '0');
        else
            break;
    }
    if (valid) {
        *valid = (i > 0);
    }
    return v;
}

Buffer getLine(const Buffer& s, size_t fromOffset)
{
    size_t offset = s.FindFirst('\n', fromOffset);
    const char *start = s.c_str() + fromOffset;

    if (offset == Buffer::NotFound)
        return Buffer(start);
    return Buffer(start, offset - fromOffset + 1);
}

unsigned int parseDottedDecimal(const char *src, int *dsts, unsigned int ndsts)
{
    unsigned int i = 0;
    bool reset = true;

    while (i < ndsts) {
        if (*src >= '0' && *src <= '9') {
            if (reset) {
                dsts[i] = 0;
                reset = false;
            }
            dsts[i] = (dsts[i] * 10) + (*src - '0');
        } else if (*src == '.') {
            reset = true;
            ++i;
        } else {
            if (!reset)
                ++i;
            break;
        }
        ++src;
    }
    return i;
}

int trimLeading(Buffer& s, char c)
{
    char setOfChars[] = { c, 0 };

    return trimLeadingOfSet(s, setOfChars);
}

int trimLeadingOfSet(Buffer& s, const char *setOfChars)
{
    const char *data = s.c_str();
    size_t len = s.length();
    size_t toTrim = 0;

    while (toTrim < len && strchr(setOfChars, data[toTrim])) {
        toTrim++;
    }
    s.remove(0, toTrim);
    return toTrim;
}

bool trimLeading(Buffer& haystack, const Buffer& needle)
{
    if (startsWith(haystack, needle)) {
        haystack.remove(0, needle.length());
        return true;
    }
    return false;
}

void trimTrailing(Buffer& s, char c)
{
    char setOfChars[] = { c, 0 };

    trimTrailingOfSet(s, setOfChars);
}

void trimTrailingOfSet(Buffer& s, const char *setOfChars)
{
    const char *data = s.c_str();
    size_t len = s.length();
    size_t newLen = len;

    while (newLen > 0 && strchr(setOfChars, data[newLen - 1])) {
        newLen--;
    }
    s.truncate(newLen);
}

void trimWhitespace(Buffer& s)
{
    trimTrailingOfSet(s, " \t\v\r\n");
    trimLeadingOfSet(s, " \t\v\r\n");
}

bool startsWith(Buffer& haystack, const Buffer& needle)
{
    return haystack.length() >= needle.length() &&
           memcmp(haystack.c_str(), needle.c_str(), needle.length()) == 0;
}
#endif

}

std::string formatList(const char *fmt, va_list argList)
{
    std::string s;

#if 1 // TODO:  HAVE_VASPRINTF
    char *buf;
    int len = vasprintf(&buf, fmt, argList);
    if (len >= 0) {
        s.assign(buf, len);
        free(buf);
    }
#else
    va_list argList2;
    va_copy(argList2, argList);
    int len = vsnprintf(nullptr, 0, fmt, argList2) + 1;  // measure,
    va_end(argList2);
    char *buf = (char *)alloca(len);
    int printed = vsnprintf(buf, len, fmt, argList); // format,
    assert(printed + 1 == len);
    (void)printed;
    _DetachWith(buf, len - 1, len - 1);              // copy
#endif
    return s;
}

std::string format(const char *fmt, ...)
{
    va_list argList;

    va_start(argList, fmt);
    std::string s = formatList(fmt, argList);
    va_end(argList);
    return s;
}

std::string& appendFormatList(std::string& s, const char *fmt, va_list argList)
{
#if 1  // TODO:  HAVE_VASPRINTF
    char *buf;
    int len = vasprintf(&buf, fmt, argList);
    if (len >= 0) {
        s.append(buf, len);
        free(buf);
    }
#else
    va_list argList2;
    va_copy(argList2, argList);
    int len = vsnprintf(nullptr, 0, fmt, argList2) + 1;  // measure,
    va_end(argList2);
    char *buf = (char *)alloca(len);
    int printed = vsnprintf(buf, len, fmt, argList); // format,
    assert(printed + 1 == len);
    (void)printed;
    _DoAppend(buf, len - 1);                         // copy
#endif
    return s;
}

std::string& appendFormat(std::string& s, const char *fmt, ...)
{
    va_list argList;

    va_start(argList, fmt);
    appendFormatList(s, fmt, argList);
    va_end(argList);
    return s;
}

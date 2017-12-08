#include "util/Debug.h"

#include "util/Buffer.h"
#include "util/StrUtil.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#if defined(__linux__)
#include <sys/prctl.h>
#endif


int Debugger::enter()
{
    volatile int r = 0;

#ifdef HAVE_BUILTIN_TRAP
    __builtin_trap();
#else
    abort();
#endif
    return r;
}

void Debugger::print(char const *str)
{
    printf("%s", str);
}

void Debugger::printf(const Buffer &b)
{
    Debugger::print(b.c_str());
}

void Debugger::printf(char const *fmt, va_list ap)
{
    vprintf(fmt, ap);  // TODO:  various destinations
}

void Debugger::printf(char const *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    Debugger::printf(fmt, ap);
    va_end(ap);
}

int Debugger::asserted(char const *file, int line, char const *expr)
{
    Debugger::printf("%s:%d: failed assertion `%s'\n", file, line, expr);
    int r = Debugger::enter();

    if (r == 0)
        abort();
    return r;
}

void Debugger::nameThread(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::string name = formatList(fmt, ap);
    va_end(ap);

#if DEBUG
#if defined(__linux__)
    prctl(PR_SET_NAME, (unsigned long)name.c_str());
#endif
#endif
}

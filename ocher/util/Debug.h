#ifndef LIBCLC_DEBUG_H
#define LIBCLC_DEBUG_H

/** @file
 *  Debugger access.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>


class Buffer;

class Debugger {
public:
    /**
     *  @return  Arbitrary value set by debugger; normally 0
     */
    static int enter();

    static void print(char const *str);

    static void printf(const Buffer &b);

    static void printf(const char *fmt, va_list ap);

    static void printf(char const *fmt, ...);

    static int asserted(char const *file, int line, char const *expr);

    /**
     * Names the thread, so that the name is visible in the debugger.
     * @note only in debug builds
     */
    static void nameThread(const char *name);
};

#if DEBUG

#define ASSERT(E)  do { if (!(E)) Debugger::asserted(__FILE__, __LINE__, #E); } while (0)
#define ASSERT_WITH_MESSAGE(E, msg) \
    do { if (!(E)) Debugger::asserted(__FILE__, __LINE__, msg); } while (0)

#else

#define ASSERT(E)                    (void)0
#define ASSERT_WITH_MESSAGE(E, msg)  (void)0

#endif

#define UNUSED(x) (void)(sizeof((x), 0))

#endif

#if defined(__CYGWIN__) || defined(_WIN32)
//#define USE_CLC_MEMRCHR
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h>
#if defined(BSD)
    /* BSD (DragonFly BSD, FreeBSD, OpenBSD, NetBSD). ----------- */
#endif

#endif

/**
 * usage:
 * LD_PRELOAD=/path/to/libioctldumper.so /bin/ebrmain
 */
#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>

static int _fd = -1;

int open(const char *pathname, int flags, mode_t mode)
{
    static int (*func)(const char *, int, mode_t);

    if (!func) {
        func = (int (*)(const char *, int, mode_t)) dlsym(RTLD_NEXT, "open");
    }

    int fd = func(pathname, flags, mode);
    if (fd >= 0 && strcmp(pathname, "/dev/fb0") == 0) {
        printf("open(%s, %d, %d) = %d\n", pathname, flags, mode, fd);
        _fd = fd;
    }

    return fd;
}

int close(int fd)
{
    static int (*func)(int);

    if (!func) {
        func = (int (*)(int)) dlsym(RTLD_NEXT, "close");
    }

    if (fd == _fd) {
        printf("close(%d)\n", _fd);
        _fd = -1;
    }

    return func(fd);
}

int ioctl(int fd, int request, ...)
{
    static int (*func)(int fd, int request, ...);

    if (!func) {
        func = (int (*)(int d, int request, ...)) dlsym(RTLD_NEXT, "ioctl");
    }

    va_list args;

    va_start(args, request);
    void *p = va_arg(args, void *);
    va_end(args);
    unsigned int *x = (unsigned int *) p;

    if (_fd == fd) {
        int i;
        printf("ioctl(%d, 0x%x, %p)\n", fd, request, p);

        if (request >= 0x4539 && request <= 0x4559) {
            /* partial image update */

            /**
             * struct ioctl_command {
             *	u32 x;
             * 	u32 y;
             *	u32 width;
             *	u32 height;
             *	uchar buf[800 * 600];
             * };
             */
            printf("\tpartial update: %u %u %u %u %u %u ", x[0], x[1], x[2], x[3], x[4], x[5]);
            printf("\n");
        } else if (request == 0x4540) {
            /* status ??? */

            /**
             * u32 my32 = ???;
             * ioctl(/dev/fb0, 0x4540, &my_u32);
             */
            printf("\tstatus: %u %u %u %u %u %u ", x[0], x[1], x[2], x[3], x[4], x[5]);
            printf("\n");
        } else {
            printf("\t???: ");
            unsigned int *y = (unsigned int *)p;
            for (i = 0; i < 17; i++) {
                printf("%x ", y[i]);
            }
            printf("\n");
        }
    }

    int rc = func(fd, request, p);
    printf("\t== %d", rc);
    printf("\t???: %u %u %u %u %u %u\n", x[0], x[1], x[2], x[3], x[4], x[5]);

    return rc;
}

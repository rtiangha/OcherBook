#ifndef OCHER_INTRINSICS_H
#define OCHER_INTRINSICS_H

#if __GNUC__ >= 3
#define likely(cond)    __builtin_expect(!!(cond), 1)
#define unlikely(cond)  __builtin_expect(!!(cond), 0)
#else
#define likely(cond)   (cond)
#define unlikely(cond) (cond)
#endif

#endif

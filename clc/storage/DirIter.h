#ifndef LIBCLC_DIRITER_H
#define LIBCLC_DIRITER_H

#include <stdint.h>
#ifndef _WIN32
#include <sys/stat.h>
#include <dirent.h>
#endif

#include "clc/data/Buffer.h"


namespace clc
{

/**
 *  Directory iterator.
 */
class DirIter
{
public:
    enum {
        IMPLICIT = 1,  ///< Include the implicit '.' and '..' entries
        FOLLOW_LINK = 2,
    };

    DirIter();

    DirIter(const char* dir, unsigned int flags=0);

    ~DirIter();

    void setTo(const char* dir, unsigned int flags=0);

    /**
     * @param[out] entryName  The name of the next entry, or empty string if no more entries.
     * @return Zero success (including end-of-iter), or nonzero on error.  More specifically:
     *     Zero, entryName not empty: entry was sucessfully retrieved.
     *     Zero, entryName empty: end of iteration.
     *     Nonzero, entryName empty: iteration failed.
     */
    int getNext(Buffer& entryName);

    const Buffer getName() const { return m_name; }

protected:
    void close();

    Buffer m_name;
    unsigned int m_flags;
#ifdef _WIN32
    HANDLE m_dp;
#else
    DIR* m_dp;
#endif
};

}

#endif


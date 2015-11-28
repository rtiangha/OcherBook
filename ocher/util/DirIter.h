#ifndef LIBCLC_DIRITER_H
#define LIBCLC_DIRITER_H

#include <dirent.h>
#include <stdint.h>
#include <string>
#include <sys/stat.h>


/**
 *  Directory iterator.
 */
class DirIter {
public:
    enum {
        IMPLICIT = 1,  ///< Include the implicit '.' and '..' entries
        FOLLOW_LINK = 2,
    };

    DirIter();

    DirIter(const char *dir, unsigned int flags = 0);

    ~DirIter();

    void setTo(const char *dir, unsigned int flags = 0);

    /**
     * @param[out] entryName  The name of the next entry, or empty string if no more entries.
     * @return Zero success (including end-of-iter), or nonzero on error.  More specifically:
     *     Zero, entryName not empty: entry was sucessfully retrieved.
     *     Zero, entryName empty: end of iteration.
     *     Nonzero, entryName empty: iteration failed.
     */
    int getNext(std::string &entryName);

    const std::string &getName() const
    {
        return m_name;
    }

protected:
    void close();

    std::string m_name;
    unsigned int m_flags;
    DIR *m_dp;
};

#endif

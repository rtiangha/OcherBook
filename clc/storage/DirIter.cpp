#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
#include <shlobj.h>
#else
#include <dirent.h>
#include <fnmatch.h>
#endif

#include "clc/data/Buffer.h"
#include "clc/storage/DirIter.h"
#include "clc/storage/Path.h"


namespace clc
{

DirIter::DirIter() : m_flags(0), m_dp(0)
{
}

DirIter::DirIter(const char* dir, unsigned int flags) : m_name(dir), m_flags(flags), m_dp(0)
{
}

DirIter::~DirIter()
{
    close();
}

void DirIter::close()
{
#ifdef _WIN32
    if (m_dp) {
        FindClose(m_dp);
        m_dp = 0;
    }
#else
    if (m_dp) {
        closedir(m_dp);
        m_dp = 0;
    }
#endif
}

void DirIter::setTo(const char* dir, unsigned int flags)
{
    close();
    m_name = dir;
    m_flags = flags;
}

int DirIter::getNext(Buffer& entryName)
{
    entryName.clear();
    if (!m_dp && !m_name.length())
        return ENOTDIR;
#ifdef _WIN32
#define SHOULD_SKIP(name) ((m_flags&IMPLICIT)==0 && (strcmp((name), ".")==0 || strcmp((name), "..")==0))
    WIN32_FIND_DATA fd;
    char* name = &fd.cFileName[0];
    if (!m_dp) {
        Buffer glob = Path::join(m_name.c_str(), "*");
        m_dp = FindFirstFile(glob, &fd);
        if (m_dp) {
            if (! SHOULD_SKIP(name))
                goto done;
            // else fall through to FindNextFile
        } else
            goto err;
    }
    do {
        if (FindNextFile(m_dp, &fd) == 0)
            goto err;
    }  while (SHOULD_SKIP(name));
done:
    entryName = name;
    return 0;
err:
    int error = GetLastError();
    if (error == ERROR_NO_MORE_FILES)
        error = 0;
    return error;
#undef SHOULD_SKIP
#else
    if (! m_dp) {
        m_dp = opendir(m_name);
        if (! m_dp)
            return errno;
    }
    errno = 0;
    struct dirent* de;
    do {
        de = readdir(m_dp);
    } while (de && !(m_flags & IMPLICIT) && (strcmp(de->d_name, ".")==0 || strcmp(de->d_name, "..")==0));
    if (de) {
        entryName = de->d_name;
        return 0;
    } else {
        return errno;
    }
#endif
}

}


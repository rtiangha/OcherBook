#include "util/DirIter.h"

#include "util/Path.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <sys/types.h>


DirIter::DirIter() :
    m_flags(0),
    m_dp(nullptr)
{
}

DirIter::DirIter(const char* dir, unsigned int flags) :
    m_name(dir),
    m_flags(flags),
    m_dp(nullptr)
{
}

DirIter::~DirIter()
{
    close();
}

void DirIter::close()
{
    if (m_dp) {
        closedir(m_dp);
        m_dp = nullptr;
    }
}

void DirIter::setTo(const char* dir, unsigned int flags)
{
    close();
    m_name = dir;
    m_flags = flags;
}

int DirIter::getNext(std::string& entryName)
{
    entryName.clear();
    if (!m_dp && !m_name.length())
        return ENOTDIR;
    if (!m_dp) {
        m_dp = opendir(m_name.c_str());
        if (!m_dp)
            return errno;
    }
    errno = 0;
    struct dirent* de;
    do {
        de = readdir(m_dp);
    } while (de && !(m_flags & IMPLICIT) && (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0));
    if (de) {
        entryName = de->d_name;
        return 0;
    } else {
        return errno;
    }
}

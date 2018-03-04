#include "util/Path.h"

#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <fnmatch.h>
#include <string>

#define PATH_SEP '/'

bool Path::isAbsolute(const char *path)
{
    return *path == PATH_SEP;
}

std::string Path::join(const char *base, const char *leaf)
{
    std::string path;

    if (!leaf) {
        path = base;
    } else if (isAbsolute(leaf)) {
        path = leaf;
    } else {
        path = base;
        if (path.length() && path[path.length() - 1] != PATH_SEP)
            path += PATH_SEP;
        path += leaf;
    }
    return path;
}

int Path::list(const char *dir, const char *glob, std::list<std::string> &files)
{
    int r = -1;

    try {
        DIR *dp;
        if ((dp = opendir(dir)) == nullptr) {
            r = errno;
        } else {
            r = 0;
            struct dirent *dir_entry;
            while ((dir_entry = readdir(dp)) != nullptr) {
                const char *name = dir_entry->d_name;
                if (!glob || fnmatch(glob, name, 0) == 0) {
                    files.emplace_back(std::string(name));
                }
            }
            closedir(dp);
        }
    } catch (std::bad_alloc &) {
        r = ENOMEM;
    }
    return r;
}

std::string Path::getDirectory(std::string &path)
{
    std::string dir(path);
    size_t pos = path.rfind(PATH_SEP);

    if (pos != std::string::npos)
        dir.resize(pos + 1);
    return dir;
}

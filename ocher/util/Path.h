#ifndef UTIL_PATH_H
#define UTIL_PATH_H

#include <list>
#include <string>

class Path {
public:
    /**
     *  @return  True if the path is an absolute path on this platform.
     */
    static bool isAbsolute(const char *path);

    /**
     *  @param base  The base pathname.
     *  @param leaf  The portion to be appended.  Note that absolute paths override
     *      existing data in base.
     *  @return The combined pathname.
     */
    static std::string join(const char *base, const char *leaf);

    /**
     *  @param base  The base pathname.
     *  @param leaf  The portion to be appended.  Note that absolute paths override
     *      existing data in base.
     *  @return The combined pathname.
     */
    static inline std::string join(const std::string &base, const char *leaf)
    {
        return join(base.c_str(), leaf);
    }

    /**
     *  Returns a list of files that match a pattern in a directory.  Note that all matching names
     *  are returned regardless of type (directory, regular file, link, etc).
     *  @param directory  The directory in which to list files
     *  @param glob  The filename globbing pattern, using literal characters, ?, and *.
     *      nullptr implies * (all files).
     *  @param files  Returned list of files.
     *  @return 0 on success; nonzero for an error (files list may be partial).  Even on error,
     *      caller is responsbile for cleaning up files list.
     */
    static int list(const char *directory, const char *glob, std::list<std::string> &files);

    static std::string getDirectory(std::string &path);
};

#endif

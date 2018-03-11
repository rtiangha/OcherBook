/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UNZIP_CACHE_H
#define OCHER_UNZIP_CACHE_H

#include "fmt/epub/TreeMem.h"
#include "fmt/minizip/unzip.h"

#include <list>
#include <string>


class FileCache {
public:
    virtual ~FileCache()
    {
    }

    virtual TreeFile* getFile(const char* filename, const char* relative = 0) = 0;
    virtual TreeDirectory* getRoot() = 0;
};

/**
 * Unzips zip files to memory on-demand, and caches the results.
 */
class UnzipCache : public FileCache {
public:
    UnzipCache(const char* zipFilename, const char* password = 0);
    ~UnzipCache();

    TreeFile* getFile(const char* filename, const char* relative = 0);

    TreeDirectory* getRoot()
    {
        return m_root;
    }

protected:
    void clearCache();
    void newCache();

    /**
     * @param pattern  If nullptr, extract all, otherwise extract only the named file(s).
     * @param matchedName  The actual name that matched pattern (even if pattern is nullptr, and even
     *      if the extraction failed)
     * @return -1 error, 0 did not match, 1 matched and extracted, 2 matched uniquely, quit now
     */
    int unzipFile(const char* pattern, std::string* matchedName);

    /**
     * Unzips file(s) that match the pattern into the TreeDirectory.
     * @param pattern  If nullptr, extract all, otherwise extract only the named file(s).
     * @param matchedNames  If not nullptr, extracted pathnames are appended
     * @return -1 stopped due to error, else number matched
     */
    int unzip(const char* pattern, std::list<std::string>* matchedNames);

    unzFile m_uf;
    TreeDirectory* m_root;
    std::string m_filename;
    std::string m_password;
};

#endif

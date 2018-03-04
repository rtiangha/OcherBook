/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "fmt/epub/UnzipCache.h"

#include "util/Buffer.h"
#include "util/Logger.h"
#include "util/Path.h"

#include <fnmatch.h>

#define LOG_NAME "ocher.epub.unzip"


UnzipCache::UnzipCache(const char* filename, const char* password) :
    m_uf(nullptr),
    m_root(nullptr),
    m_filename(filename),
    m_password(password ? password : "")
{
    newCache();
}

UnzipCache::~UnzipCache()
{
    if (m_uf)
        unzClose(m_uf);
    clearCache();
}

void UnzipCache::newCache()
{
    m_root = new TreeDirectory(".");
}

void UnzipCache::clearCache()
{
    if (m_root) {
        delete m_root;
        m_root = nullptr;
    }
}

TreeFile* UnzipCache::getFile(const char* filename, const char* relative)
{
    std::string fullPath;

    if (relative) {
        fullPath = Path::join(relative, filename);
        filename = fullPath.c_str();
    }
    TreeFile* f = m_root->findFile(filename);
    if (f)
        return f;
    if (unzip(filename, nullptr)) {
        // Even if error, may have extracted.
        f = m_root->findFile(filename);
    }
    return f;
}

int UnzipCache::unzipFile(const char* pattern, std::string* matchedName)
{
    char pathname[256];
    int err;

    unz_file_info64 file_info;

    err = unzGetCurrentFileInfo64(m_uf, &file_info, pathname, sizeof(pathname), nullptr, 0, nullptr, 0);
    if (err != UNZ_OK) {
        Log::error(LOG_NAME, "unzGetCurrentFileInfo: %d", err);
        return -1;
    }

    int match = 1;
    if (pattern) {
        // TODO:  allow match to be looser:  leading ./, \, etc.  Anything but wildcards.
        if (strcmp(pattern, pathname) == 0)
            match = 2;
        int r = fnmatch(pattern, pathname, FNM_NOESCAPE /*| FNM_CASEFOLD*/);
        if (r == 0) {
            Log::trace(LOG_NAME, "matched %s to %s", pathname, pattern);
            if (matchedName)
                matchedName->assign(pathname);
        } else if (r == FNM_NOMATCH) {
            // Log::trace(LOG_NAME, "did not match %s to %s", pathname, pattern);
            return 0;
        } else {
            Log::error(LOG_NAME, "fnmatch: %s: error", pattern);
            return -1;
        }
    }

    Buffer buffer;
    std::string filename;
    TreeFile* tfile = nullptr;

    char* start = pathname;
    char* p = start;
    TreeDirectory* root = m_root;
    while (true) {
        if (*p == '/' || *p == '\\' || *p == 0) {
            if (p - start) {
                std::string name(start, p - start);
                if (*p) {
                    root = root->createDirectory(name);
                } else {
                    tfile = root->createFile(name, buffer);
                    filename = name;
                    Log::trace(LOG_NAME, "Creating file %s", filename.c_str());
                }
            }
            if (!*p)
                break;
            start = p + 1;
        }
        p++;
    }
    ;

    if (tfile) {
        char* buf = buffer.lockBuffer(file_info.uncompressed_size);

        err = unzOpenCurrentFilePassword(m_uf, m_password.empty() ? nullptr : m_password.c_str());
        if (err != UNZ_OK) {
            Log::error(LOG_NAME, "unzOpenCurrentFilePassword: %d", err);
        } else {
            Log::trace(LOG_NAME, "extracting: %s", pathname);

            do {
                err = unzReadCurrentFile(m_uf, buf, buffer.size());
                if (err < 0) {
                    Log::error(LOG_NAME, "unzReadCurrentFile: %d", err);
                }
            } while (err > 0);
        }
        buffer.unlockBuffer(file_info.uncompressed_size);
        tfile->data = buffer;

        if (err == UNZ_OK) {
            err = unzCloseCurrentFile(m_uf);
            if (err != UNZ_OK) {
                Log::error(LOG_NAME, "unzCloseCurrentFile: %d", err);
            }
        } else
            unzCloseCurrentFile(m_uf);    /* don't lose the error */
    }

    return err == UNZ_OK ? match : -1;
}

int UnzipCache::unzip(const char* pattern, std::list<std::string>* matchedNames)
{
    uLong i;
    unz_global_info64 gi;
    int numMatched = 0;

    if (m_uf)
        unzClose(m_uf);
    m_uf = unzOpen64(m_filename.c_str());
    int err = unzGetGlobalInfo64(m_uf, &gi);
    if (err != UNZ_OK) {
        Log::error(LOG_NAME, "unzGetGlobalInfo: %d", err);
        return -1;
    }

    for (i = 0; i < gi.number_entry; i++) {
        int r;
        if (matchedNames) {
            std::string matchedName;
            r = unzipFile(pattern, &matchedName);
            if (!matchedName.empty())
                matchedNames->push_back(matchedName);
        } else {
            r = unzipFile(pattern, nullptr);
        }
        if (r > 0) {
            ++numMatched;
            if (r > 1)
                break;
        }

        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(m_uf);
            if (err != UNZ_OK) {
                Log::error(LOG_NAME, "unzGoToNextFile: %d", err);
                return -1;
            }
        }
    }

    return numMatched;
}

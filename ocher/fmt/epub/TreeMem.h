/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_EPUB_TREEMEM_H
#define OCHER_EPUB_TREEMEM_H

/** @file
 * Represents a simple filesystem in memory.
 */

#include "util/stdex.h"

#include <cstring>
#include <list>
#include <string>

class TreeFile {
public:
    TreeFile(const std::string& _name) :
        name(_name)
    {
    }

    TreeFile(const std::string& _name, const std::string& _data) :
        name(_name),
        data(_data)
    {
    }

    const std::string name;
    std::string data;
    // error
};

class TreeDirectory {
public:
    TreeDirectory(const char* _name) :
        name(_name)
    {
    }

    TreeDirectory(const std::string& _name) :
        name(_name)
    {
    }

    ~TreeDirectory() = default;

    std::string name;

    std::list<std::unique_ptr<TreeDirectory>> subdirs;
    std::list<std::unique_ptr<TreeFile>> files;

    TreeFile* createFile(const std::string& _name, const std::string& _data = {})
    {
        TreeFile* file = getFile(_name);

        if (!file) {
            auto f = make_unique<TreeFile>(_name, _data);
            file = f.get();
            files.push_back(std::move(f));
        }
        return file;
    }

    TreeDirectory* createDirectory(const std::string& _name)
    {
        TreeDirectory* dir = getDirectory(_name);

        if (!dir) {
            auto d = make_unique<TreeDirectory>(_name);
            dir = d.get();
            subdirs.push_back(std::move(d));
        }
        return dir;
    }

    TreeFile* getFile(const std::string& _name) const
    {
        for (auto& file : files) {
            if (file->name == _name)
                return file.get();
        }
        return nullptr;
    }

    TreeDirectory* getDirectory(const std::string& _name) const
    {
        for (auto& subdir : subdirs) {
            if (subdir->name == _name)
                return subdir.get();
        }
        return nullptr;
    }

    TreeFile* findFile(const char* _name) const
    {
        const char* slash = strchr(_name, '/');

        if (slash) {
            size_t len = slash - _name;
            for (auto& subdir : subdirs) {
                if (subdir->name.length() == len && strncmp(subdir->name.c_str(), _name, len) == 0) {
                    return subdir->findFile(slash + 1);
                }
            }
        } else {
            for (auto& file : files) {
                if (file->name == _name) {
                    return file.get();
                }
            }
        }
        return nullptr;
    }
};

#endif

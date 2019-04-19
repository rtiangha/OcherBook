/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_EPUB_PARSER_H
#define OCHER_EPUB_PARSER_H

#include "fmt/Format.h"
#include "fmt/epub/UnzipCache.h"

#include <mxml.h>

#include <map>
#include <string>
#include <vector>


struct EpubItem {
    std::string href;
    std::string mediaType;
};

/** EPub file format reader
 */
class Epub : public Format {
public:
    /**
     */
    Epub(FileCache* fileCache);

    /**
     */
    Epub(const std::string& epubFilename, const char* password = nullptr);

    ~Epub();

    std::string m_epubVersion;
    std::string m_uid;

    std::string getFile(const char* filename)
    {
        TreeFile* f = m_zip->getFile(filename, m_contentPath.c_str());
        std::string b;

        if (f) {
            b = f->data;
        }
        return b;
    }

    int getSpineItemByIndex(unsigned int i, std::string& item);
    int getManifestItemById(unsigned int i, std::string& item);
    int getContentByHref(const char* href, std::string& item);

    /** Parses XML.
     *
     * Caller must call mxml_delete.
     */
    mxml_node_t* parseXml(std::string& xml);

protected:
    TreeFile* findSpine();
    void parseSpine(TreeFile* spine);

    // TODO:OWNERSHIP unique_ptr
    FileCache* m_zip;
    std::map<std::string, EpubItem> m_items;
    std::vector<std::string> m_spine;
    std::string m_contentPath;  ///< directory of full-path attr
};


#endif

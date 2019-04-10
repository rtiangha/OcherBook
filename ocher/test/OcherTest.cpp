/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#undef LOG_LEVEL
#define LOG_LEVEL 5

#include "Container.h"
#include "device/Battery.h"
#include "device/Device.h"
#include "fmt/Format.h"
#include "fmt/epub/TreeMem.h"
#include "settings/Options.h"
#include "util/LogAppenders.h"
#include "util/Path.h"
#include "util/StrUtil.h"
#include "ux/PowerSaver.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define REQUIRE_RESOURCES_DIR \
    if (!resourcesDir) { \
        printf("\tSpecify resources directory on command line; skipping test...\n"); \
        return; \
    }

char *resourcesDir;
Options opt;

TEST_CASE("PowerSaver") {
}

TEST_CASE("Battery Capacity", "[Battery]") {
    Battery b;
    int r;

    r = b.readCapacity();
    if (r == -1) {
        CHECK(b.percent == -1);
    } else {
        CHECK(b.percent >= 0);
        CHECK(b.percent <= 100);
    }
}

TEST_CASE("Battery Status", "[Battery]") {
    Battery b;
    int r;

    r = b.readStatus();
    if (r == -1) {
        CHECK(b.status == Battery::Status::Unknown);
    } else {
        if (b.status != Battery::Status::Charging && b.status != Battery::Status::Discharging)
            CHECK(b.status == Battery::Status::Charging);  // catch only does binary comparisons
    }
}

TEST_CASE("Detect format") {
    REQUIRE_RESOURCES_DIR;

    Buffer fn;
    Encoding enc;
    Fmt fmt;

    fn = resourcesDir;
    fn += "/";
    fn += "simple.txt";
    fmt = detectFormat(fn.c_str(), &enc);
    CHECK(enc == OCHER_ENC_UTF8);
    CHECK(fmt == OCHER_FMT_TEXT);

    fn = resourcesDir;
    fn += "/";
    fn += "simple.bin";
    fmt = detectFormat(fn.c_str(), &enc);
    CHECK(enc == OCHER_ENC_UNKNOWN);
    CHECK(fmt == OCHER_FMT_UNKNOWN);
    fn = resourcesDir;

    fn += "/";
    fn += "simple.html";
    fmt = detectFormat(fn.c_str(), &enc);
    CHECK(enc == OCHER_ENC_UTF8);
    CHECK(fmt == OCHER_FMT_HTML);

    // epub
}

TEST_CASE("Epub") {
    class EpubBuilder : public FileCache {
    public:
        EpubBuilder() :
            m_root(make_unique<TreeDirectory>("."))
        {
            Buffer data("application/epub+zip");
            m_root->createFile("mimetype", data);
        }

        TreeFile *getFile(const char *filename, const char *relative = 0) override
        {
            std::string fullPath;

            if (relative) {
                fullPath = Path::join(relative, filename);
                filename = fullPath.c_str();
            }
            return m_root->findFile(filename);
        }

        TreeDirectory *getRoot() override
        {
            return m_root.get();
        }

        void createContainerXml(const char *contentOpfPath)
        {
            TreeDirectory *metaInfDir = m_root->createDirectory("META-INF");
            auto containerXml = format(
                    "<?xml version=\"1.0\"?>"
                    "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">"
                    "<rootfiles>"
                    "<rootfile full-path=\"%s\" media-type=\"application/oebps-package+xml\"/>"
                    "</rootfiles>"
                    "</container>", contentOpfPath);
            Buffer b(containerXml.c_str());
            metaInfDir->createFile("container.xml", b);
        }

        void createContentOpf(TreeDirectory *containingDir, const char *title)
        {
            auto contentOpf = format(
                    "<?xml version=\"1.0\"?>"
                    "<package xmlns=\"http://www.idpf.org/2007/opf\" unique-identifier=\"BookID\" version=\"2.0\">"
                    "    <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">"
                    "        <dc:title>%s</dc:title>"
                    "    </metadata>"
                    "    <manifest>"
                    "        <item id=\"ncx\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\"/>"
                    "    </manifest>"
                    "    <spine toc=\"ncx\">"
                    "    </spine>"
                    "</package>", title);
            Buffer b(contentOpf.c_str());
            containingDir->createFile("content.opf", b);
        }

        std::unique_ptr<TreeDirectory> m_root;
    };

    SECTION("Detect") {
        EpubBuilder *c = new EpubBuilder;

        c->createContainerXml("content.opf");
        TreeDirectory *root = c->getRoot();
        TreeDirectory *textDir = root->createDirectory("text");
        (void)textDir; // TODO
        c->createContentOpf(root, "Testing Book");

        Epub epub(c);
    }
}

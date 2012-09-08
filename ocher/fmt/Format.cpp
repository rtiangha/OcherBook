#include "clc/storage/File.h"

#include "ocher/fmt/Format.h"


Fmt detectFormat(const char* file)
{
    Fmt format = OCHER_FMT_UNKNOWN;
    Encoding encoding = OCHER_ENC_UNKNOWN;

    clc::File f;
    if (f.setTo(file) == 0) {
        unsigned char buf[4];
        ssize_t r = f.read((char*)buf, 4);
        if (r >= 4 && buf[0] == 'P' && buf[1] == 'K' && buf[2] == 0x03 && buf[3] == 0x04) {
            format = OCHER_FMT_EPUB;
        } else {
            int skip;
            if (r >= 3 && buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf) {
                skip = 3;
                encoding = OCHER_ENC_UTF8;
            } else if (r >= 4 && buf[0] == 0xff && buf[1] == 0xfe && (buf[2] || buf[3])) {
                skip = 2;
                encoding = OCHER_ENC_UTF16LE;
            } else if (r >= 4 && buf[0] == 0xfe && buf[1] == 0xff && (buf[2] || buf[3])) {
                skip = 2;
                encoding = OCHER_ENC_UTF16BE;
            } else if (r >= 4 && buf[0] == 0xff && buf[1] == 0xfe && !buf[2] && !buf[3]) {
                skip = 4;
                encoding = OCHER_ENC_UTF32LE;
            } else if (4 >= 4 && !buf[0] && !buf[1] && buf[2] == 0xfe && buf[3] == 0xff) {
                skip = 4;
                encoding = OCHER_ENC_UTF32BE;
            } else {
                // Recommended for UTF8 to not have BOM.  Also handles plain ascii.
                skip = 0;
                encoding = OCHER_ENC_UTF8;
            }

            unsigned char buf2[512];
            f.seek(skip, SEEK_SET);
            r = f.read((char*)buf2, sizeof(buf2));
            if (r >= 0) {
                // TODO:  real heuristic
                if (r > 0 && buf2[0] == '<')
                    format = OCHER_FMT_HTML;
                else {
                    format = OCHER_FMT_TEXT;
                    for (int i = 0; i < r; ++i) {
                        if (!buf2[i]) {
                            format = OCHER_FMT_UNKNOWN;
                            break;
                        }
                    }
                }
            }
        }
    }

    return format;
}


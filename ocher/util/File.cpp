#include "util/File.h"

#include "util/Buffer.h"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <random>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <system_error>
#include <unistd.h>

#define DEF_CREAT_MODE 0666


File::File() :
    m_file(nullptr),
    m_temp(false)
{
}

File::File(std::string pathname, const char* mode) :
    m_file(nullptr),
    m_filename(std::move(pathname)),
    m_temp(false)
{
    int r = init(mode);

    if (r)
        throw std::system_error(r, std::system_category(), "init");
}

File::File(const char* pathname, const char* mode) :
    m_file(nullptr),
    m_filename(pathname),
    m_temp(false)
{
    int r = init(mode);

    if (r)
        throw std::system_error(r, std::system_category(), "init");
}

int File::setTo(const char* pathname, const char* mode)
{
    unset();
    m_filename = pathname;
    return init(mode);
}

int File::setTo(const std::string& pathname, const char* mode)
{
    unset();
    m_filename = pathname;
    return init(mode);
}

int File::init(const char* mode)
{
    /*
     * Cannot use fopen because the strings are not completely portable (in particular, "x" to
     * fail if the file already exists is only on Mac).
     * Cannot use mkstemp because doesn't exist on Windows.
     * mktemp is near useless on Windows because it uses the PID and only generates 26 possible
     * filenames per process.
     */
    char fmode[4] = { 0 }; // Collected flags for fdopen
    int fmi = 0;
    int oflag = 0;  // Collected flags for open
    int omode = DEF_CREAT_MODE;

    int state = 0;  // Trivial FSM: 0=r,w,a 1=+ 2=x,u,t
    int mi = 0;
    char c;
    bool uniq = false;

    while ((c = mode[mi++])) {
        switch (state) {
        case 0: {
            if (c == 'r') {
                oflag |= O_RDONLY;
            } else if (c == 'w') {
                oflag |= (O_WRONLY | O_CREAT | O_TRUNC);
            } else if (c == 'a') {
                oflag |= (O_WRONLY | O_CREAT | O_APPEND);
            } else {
                return EINVAL;
            }
            fmode[fmi++] = c;
            ++state;
            break;
        }
        case 1: {
            if (c == 'b')
                continue;
            ++state;
            if (c == '+') {
                oflag &= ~(O_WRONLY | O_RDONLY);
                oflag |= O_RDWR;
                fmode[fmi++] = c;
                break;
            }
        }
        case 2: {
            if (c == 'b') {
                ;
            } else if (c == 'x') {
                oflag |= O_EXCL;
            } else if (c == 'u') {
                oflag |= O_EXCL;
                uniq = true;
            } else if (c == 't') {
                m_temp = true;
            } else {
                return EINVAL;
            }
            break;
        }
        default:
            assert(false);
        }
    }
    int fd;
    if (uniq) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 10 + 26 + 26);
        m_filename.append(".XXXXXX");
        const unsigned int len = m_filename.length();
        while (true) {
            do {
                for (unsigned int i = 0; i < 6; ++i) {
                    char l = dis(gen);
                    if (l >= 10 + 26)
                        l += ('a' - 10 - 26);
                    else if (l >= 10)
                        l += ('A' - 10);
                    else
                        l += '0';
                    m_filename[len - 1 - i] = l;
                }
            } while (access(m_filename.c_str(), F_OK));
            fd = open(m_filename.c_str(), oflag, omode);
            if (fd < 0 && errno == EEXIST)
                continue;
            break;
        }
    } else {
        fd = open(m_filename.c_str(), oflag, omode);
    }
    if (fd == -1) {
        return errno;
    }
    m_file = fdopen(fd, &fmode[0]);
    if (m_file == nullptr) {
        int e = errno;
        ::close(fd);
        return e;
    }
    return 0;
}

void File::unset()
{
    if (m_file) {
        close();
        if (m_temp)
            unlink(m_filename.c_str());
        m_file = nullptr;
    }
    m_filename.clear();
    m_temp = false;
}

File::~File()
{
    unset();
}

uint64_t File::position() const
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "position");
    }
    return ftell(m_file);
}

uint64_t File::seek(int64_t offset, int whence)
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "seek");
    }
    int r = fseek(m_file, offset, whence);
    if (r == -1)
        throw std::system_error(errno, std::system_category(), "seek");
    return ftell(m_file);
}

uint64_t File::size()
{
    struct stat statbuf;

    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "size");
    }
    int fd = fileno(m_file);
    int r = fstat(fd, &statbuf);
    assert(r == 0);
    (void)r;
    return statbuf.st_size;
}

void File::getTimes(time_t& accessTime, time_t& modifyTime, time_t& changeTime)
{
    struct stat statbuf;

    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "getTimes");
    }
    int fd = fileno(m_file);
    int r = fstat(fd, &statbuf);
    assert(r == 0);
    (void)r;
    accessTime = statbuf.st_atime;
    modifyTime = statbuf.st_mtime;
    changeTime = statbuf.st_ctime;
}

uint32_t File::read(char* buf, uint32_t numBytes)
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "read");
    }
    size_t r;
    r = fread(buf, 1, numBytes, m_file);
    if (r != numBytes) {
        if (ferror(m_file)) {
            throw std::system_error(EIO, std::system_category(), "fread");
        }
    }
    return r;
}

std::string File::readLine(bool keepEol, size_t maxLen)
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "readLine");
    }
    uint32_t bytesRead = 0;
    Buffer buf;
    while (true) {
        const size_t lineChunk = 2048;
        char* tmpBuf = buf.lockBuffer(bytesRead + lineChunk);
        tmpBuf += bytesRead;
        char* r = fgets(tmpBuf, lineChunk, m_file);
        if (r == nullptr) {
            if (isEof()) {
                break;
            } else {
                buf.unlockBuffer(bytesRead);
                throw std::system_error(errno, std::system_category(), "fgets");
            }
        }
        size_t len = strlen(r);
        bytesRead += len;
        if (tmpBuf[len - 1] != '\n') {
            buf.unlockBuffer(bytesRead);
            if (maxLen && len > maxLen)
                throw std::overflow_error("readLine");
        } else {
            if (!keepEol) {
                tmpBuf[len - 1] = '\0';
                bytesRead--;
            }
            break;
        }
    }
    buf.unlockBuffer(bytesRead);
    return std::string(buf.c_str(), buf.length());
}

uint32_t File::read(Buffer& s, uint32_t numBytes, uint32_t offset)
{
    uint32_t received = 0;
    char* buffer = s.lockBuffer(offset + numBytes);

    try {
        received = read(buffer + offset, numBytes);
    } catch (...) {
        s.unlockBuffer(offset + received);
        throw;
    }
    s.unlockBuffer(offset + received);
    return received;
}

void File::readRest(std::string& s)
{
    // Tempting to query size and prealloc that much, but some files in /proc claim 0 size.
    const uint32_t maxChunk = 16384;
    uint32_t offset = s.length();
    uint32_t received = 0;
    Buffer buf;

    do {
        char* buffer = buf.lockBuffer(offset + maxChunk);
        try {
            received = read(buffer + offset, maxChunk);
        } catch (...) {
            buf.unlockBuffer(offset);
            throw;
        }
        offset += received;
        buf.unlockBuffer(offset);
    } while (received == maxChunk);
    s.assign(buf.c_str(), buf.length());
}

void File::write(const char* buf, uint32_t numBytes)
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "write");
    }
    size_t r;
    r = fwrite(buf, 1, numBytes, m_file);
    if (r != numBytes) {
        throw std::system_error(EIO, std::system_category(), "write");
    }
    assert((uint32_t)r == numBytes);  // sucessful blocking IO does not do short writes
}

void File::flush()
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "flush");
    }
    fflush(m_file);
}

void File::close()
{
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
}

bool File::isEof() const
{
    if (!m_file) {
        throw std::system_error(EINVAL, std::system_category(), "isEof");
    }
    // Note: does not set errno
    return feof(m_file) > 0;
}

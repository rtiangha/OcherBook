#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>

#include "clc/support/Debug.h"
#include "clc/support/Exception.h"
#include "clc/storage/File.h"
#include "clc/storage/Path.h"
#include "clc/algorithm/Random.h"

#if defined(__linux__)
// uclibc 0.9.29 has buggy fopen("a+") + ftell.
// http://lists.busybox.net/pipermail/uclibc-cvs/2005-September/019508.html
#define UCLIBC_FTELL_BUG
#endif

#undef O_BINARY
#define O_BINARY 0
#define DEF_CREAT_MODE 0666

namespace clc
{

File::File() : m_fd(0)
{
}

File::File(Buffer const& pathname, const char *mode) : m_fd(0), m_filename(pathname), m_temp(false)
{
    int r = init(mode);
    if (r)
        throw IOException("init", r);
}

File::File(const char* pathname, const char *mode) : m_fd(0), m_filename(pathname), m_temp(false)
{
    int r = init(mode);
    if (r)
        throw IOException("init", r);
}

int File::setTo(const char* pathname, const char* mode)
{
    unset();
    m_filename = pathname;
    return init(mode);
}

int File::setTo(const Buffer& pathname, const char* mode)
{
    unset();
    m_filename = pathname;
    return init(mode);
}

int File::init(const char *mode)
{
    /*
     * Cannot use fopen because the strings are not completely portable (in particular, "x" to
     * fail if the file already exists is only on Mac).
     * Cannot use mkstemp because doesn't exist on Windows.
     * mktemp is near useless on Windows because it uses the PID and only generates 26 possible
     * filenames per process.
     */
    char fmode[4] = {0};  // Collected flags for fdopen
    int fmi = 0;
    int oflag = O_BINARY;  // Collected flags for open
    int omode = DEF_CREAT_MODE;

    int state = 0;  // Trivial FSM: 0=r,w,a 1=+ 2=x,u,t
    int mi = 0;
    char c;
    bool uniq = false;
    while ((c = mode[mi++])) {
        switch(state) {
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
                ASSERT(0);
        }
    }
    int fd;
    if (uniq) {
        clc::Random rnd;
        m_filename.append(".XXXXXX");
        const unsigned int len = m_filename.length();
        while (1) {
            do {
                for (unsigned int i = 0; i < 6; ++i) {
                    char l = rnd.nextInt(10+26+26);
                    if (l >= 10+26)
                        l += ('a'-10-26);
                    else if (l >= 10)
                        l += ('A'-10);
                    else
                        l += '0';
                    m_filename[len-1-i] = l;
                }
            } while (Path::exists(m_filename.c_str()));
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
    m_fd = fdopen(fd, &fmode[0]);
    if (m_fd == NULL) {
        int e = errno;
        ::close(fd);
        return e;
    }
#ifdef UCLIBC_FTELL_BUG
    if (oflag & O_APPEND) {
        ::fseek(m_fd, 0, SEEK_END);
    }
#endif
    return 0;
}

void File::unset()
{
    if (m_fd) {
        close();
        if (m_temp)
            clc_unlink(m_filename.c_str());
        m_fd = 0;
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
    if (!m_fd) {
        throw IOException("position", EINVAL);
    }
    return clc_tell(m_fd);
}

uint64_t File::seek(int64_t offset, int whence)
{
    if (!m_fd) {
        throw IOException("seek", EINVAL);
    }
    int r = clc_seek(m_fd, offset, whence);
    if (r == -1)
        throw IOException("seek", errno);
    return clc_tell(m_fd);
}

uint64_t File::size()
{
    struct clc_statstruct statbuf;
    if (!m_fd) {
        throw IOException("size", EINVAL);
    }
    int fd = fileno(m_fd);
    int r = clc_fstat(fd, &statbuf);
    ASSERT(r == 0); (void)r;
    return statbuf.st_size;
}

void File::getTimes(time_t& accessTime, time_t& modifyTime, time_t& changeTime)
{
    struct clc_statstruct statbuf;
    if (!m_fd) {
        throw IOException("getTimes", EINVAL);
    }
    int fd = fileno(m_fd);
    int r = clc_fstat(fd, &statbuf);
    ASSERT(r == 0); (void)r;
    accessTime = statbuf.st_atime;
    modifyTime = statbuf.st_mtime;
    changeTime = statbuf.st_ctime;
}

uint32_t File::read(char* buf, uint32_t numBytes)
{
    if (!m_fd) {
        throw IOException("read", EINVAL);
    }
    size_t r;
    r = fread(buf, 1, numBytes, m_fd);
    if (r != numBytes) {
        if (ferror(m_fd)) {
            throw IOException("fread", EIO);
        }
    }
    return r;
}

Buffer File::readLine(bool keepEol, size_t maxLen)
{
    if (!m_fd) {
        throw IOException("readLine", EINVAL);
    }
    uint32_t bytesRead = 0;
    Buffer buf;
    for (;;) {
        const size_t lineChunk = 2048;
        char * tmpBuf = buf.lockBuffer(bytesRead + lineChunk);
        tmpBuf += bytesRead;
        char *r = fgets(tmpBuf, lineChunk, m_fd);
        if (r == NULL && isEof() == false) {
            buf.unlockBuffer(bytesRead);
            throw IOException("fgets", errno);
        }
        if (isEof()) {
            break;
        }
        size_t len = strlen(r);
        bytesRead += len;
        if (tmpBuf[len-1] != '\n') {
            buf.unlockBuffer(bytesRead);
            if (maxLen && len > maxLen)
                throw BufferOverflowException();
        } else {
            if (!keepEol) {
                tmpBuf[len-1] = '\0';
                bytesRead--;
            }
            break;
        }
    }
    buf.unlockBuffer(bytesRead);
    return buf;
}

uint32_t File::read(Buffer& s, uint32_t numBytes, uint32_t offset)
{
    uint32_t received = 0;
    char* buffer = s.lockBuffer(offset+numBytes);
    try {
        received = read(buffer+offset, numBytes);
    } catch(...) {
        s.unlockBuffer(offset+received);
        throw;
    }
    s.unlockBuffer(offset+received);
    return received;
}

void File::readRest(Buffer& s)
{
    uint64_t pos = position();
    uint64_t len = size();
    if (pos < len) {
        // TODO:  loop for each size_t chunk
        size_t toRead = len - pos;
        ssize_t offset = s.length();
        char* buffer = s.lockBuffer(offset + toRead);
        size_t r = fread(buffer + offset, 1, toRead, m_fd);
        s.unlockBuffer(offset + r);
        if (r != toRead && ferror(m_fd))
            throw IOException("fread", EIO);
    }
}

void File::write(const char* buf, uint32_t numBytes)
{
    if (!m_fd) {
        throw IOException("write", EINVAL);
    }
    size_t r;
    r = fwrite(buf, 1, numBytes, m_fd);
    if (r != numBytes) {
        throw IOException("write", EIO);
    }
    ASSERT((uint32_t)r == numBytes);  // sucessful blocking IO does not do short writes
}

void File::flush()
{
    if (!m_fd) {
        throw IOException("flush", EINVAL);
    }
    fflush(m_fd);
}

void File::close()
{
    if (m_fd) {
        fclose(m_fd);
        m_fd = NULL;
    }
}

bool File::isEof() const
{
    if (!m_fd) {
        throw IOException("isEof", EINVAL);
    }
    // Note: does not set errno
    return (feof(m_fd) > 0);
}

}


#ifndef OCHER_UTIL_FILE_H
#define OCHER_UTIL_FILE_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <time.h>


/** Portable blocking file IO.  Similar to the POSIX model but with some conveniences
 *  and some portability.
 *
 *  When you construct (or otherwise initialize) a File, the file is automatically opened.
 *  The file is closed when you re-initialize or destroy the object.
 *
 *  At each initialization, you're asked to supply an "open mode" value. This is a combination of
 *  flags that tells the object whether you want to read and/or write the file, create it if it
 *  doesn't exist, truncate it, and so on.
 *
 *  Since there are portability issues with using off_t or assuming 2G or 4G max files, this code
 *  explicitly uses uint64_t for all sizes and offsets.
 *
 *  Files are always treated as binary.  No implicit conversion is performed.
 *
 *  @todo  Rationalize exceptions vs return codes
 */
class File {
public:
    /** Constructs an empty File object, which is not usable for IO.  Call setTo before using.
     */
    File();

    /** Constructor.
     *  @param filename
     *  @param mode  Similar to the mode string expected by fopen ("r", "r+", "w", etc).  For
     *      compatibility, binary may be specified with "b", but is always assumed anyway.
     *      Optional (non-C-like) suffixes include "x" (fail if file already exists), "u"
     *      (create unique filename based off specified filename), and "t" for a temporary
     *      file (automatically unlink in destructor).  In summary, mode must
     *      match [rwa]b?+?b?[xut]?
     *  @throw  std::system_error if open failed
     */
    File(std::string filename, const char* mode = "r");
    File(const char* filename, const char* mode = "r");

    /** Like constructor, but does not throw.
     */
    int setTo(const std::string& filename, const char* mode = "r");
    int setTo(const char* filename, const char* mode = "r");

    /** Closes the file, and returns the object to an uninitialized state.
     */
    void unset();

    /** Destructor; closes the file.
     */
    ~File();

    /** Returns the name of the file (as specified in the constructor, but possibly modified for
     *  unique files).
     *  @return The name of the file.
     */
    const std::string& getName() const
    {
        return m_filename;
    }

    /** @return The current position in the file.
     */
    uint64_t position() const;

    /** @param offset
     *  @param  whence  What offset is relative to
     *  @throw  std::system_error if seek failed
     *  @return  The new absolute offset
     */
    uint64_t seek(int64_t offset, int whence);

    /** Attempts to read numBytes into buffer, from the current position of the file.
     *  @param buffer
     *  @param numBytes
     *  @return  The number of bytes read.  Less than requested indicates EOF.
     *  @throw  std::system_error on error (EOF is not an error)
     */
    uint32_t read(char* buffer, uint32_t numBytes);

    /** @throw  std::system_error
     */
    void write(const char* buf, uint32_t numBytes);
    void write(const std::string& buf)
    {
        write(buf.c_str(), buf.length());
    }

    /** @return The current size, in bytes, of the file.
     */
    uint64_t size();

    /**
     */
    void getTimes(time_t& accessTime, time_t& modifyTime, time_t& changeTime);

    /** Flush any buffered writes to disk.
     */
    void flush();

    /** Closes the file.
     */
    void close();

    /** Indicates if the end-of-file condition is set.
     * @return bool true if it EOF has been reached, false otherwise
     */
    bool isEof() const;

    FILE* m_file;

protected:
    int init(const char* mode);

    std::string m_filename;
    bool m_temp;
};

#endif

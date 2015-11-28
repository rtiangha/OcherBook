#ifndef UTIL_BUFFER_H
#define UTIL_BUFFER_H

#include <atomic>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/**
 *  An efficient copy-on-write buffer, which can hold C strings or binary data.
 *  @note  This class is 8-bit clean.  If a length is required by the API, the Buffer is assumed
 *      to hold binary data and the length is honored.  If a length is not required or provided,
 *      then the Buffer is assumed to be a C string.
 *  @note  The Buffer is intended to be lightweight (copy-on-write with no internal locking).  If
 *      Buffers are intended to be shared among threads, either external locking must be used,
 *      or all threads must treat the Buffer as read-only, or after a thread gives a copy of the
 *      Buffer to another thread which may mutate it, the giving thread can only safely destroy
 *      its object but not dereference it.
 *  @todo  fork vs _detachWith  (fork is used in some places _detachWith would be
 *          better; _detachWith has dead code path; possibly combine?)
 *  @todo  inline trivial functions
 *  @todo  not all mutators fork; how to rationalize?
 */
class Buffer {
private:
    char *m_data;

public:
    /**
     * Special error value.
     */
    static const size_t NotFound = ((size_t)-1);

    Buffer();
    Buffer(const char *string);
    Buffer(const Buffer &string);
    Buffer(const char *string, size_t maxLength);
    Buffer(size_t prealloc);
    ~Buffer();

    // Drop-in for std::string:
    char *c_str()
    {
        return m_data;
    }

    const char *c_str() const
    {
        return m_data;
    }

    operator char *() {
        return m_data;
    }

    operator const char *() const
    {
        return m_data;
    }

    char *data()
    {
        return m_data;
    }

    const char *data() const
    {
        return m_data;
    }

    void clear()
    {
        setTo("", 0);
    }

    size_t length() const;

    size_t size() const
    {
        return length();
    }

    bool empty() const
    {
        return length() == 0;
    }

    // TODO: "truncate" is std::string's void resize(size_t n, char c=0);
    // TODO: "setTo" is std::string's assign

    /**
     *  "Forks" the string, so that it is guaranteed to not share any data with any other strings.
     *  This is internally automatically called by mutating functions.
     */
    void fork();

    inline bool isEmpty() const
    {
        return length() == 0;
    }

    // Assignment
    Buffer &operator=(const Buffer &string);
    Buffer &operator=(const char *string);
    Buffer &operator=(char c);

    Buffer &formatList(const char *fmt, va_list argList);
    Buffer &format(const char *fmt, ...);
    Buffer &appendFormatList(const char *fmt, va_list argList);
    Buffer &appendFormat(const char *fmt, ...);

    Buffer &setTo(const char *string);
    Buffer &setTo(const char *string, size_t maxLength);

    Buffer &setTo(const Buffer &string);
    Buffer &adopt(Buffer &from);

    Buffer &setTo(const Buffer &string, size_t maxLength);
    Buffer &adopt(Buffer &from, size_t maxLength);

    Buffer &setTo(char c, size_t count);

    // Substring copying
    Buffer &copyInto(Buffer &into, size_t fromOffset, size_t length) const;
    void copyInto(char *into, size_t fromOffset, size_t length) const;

    // Appending
    Buffer &operator+=(const Buffer &string);
    Buffer &operator+=(const char *string);
    Buffer &operator+=(char c);

    Buffer &append(const Buffer &string);
    Buffer &append(const char *string);

    Buffer &append(const Buffer &string, size_t length);
    Buffer &append(const char *string, size_t length);
    Buffer &append(char c, size_t count);

    // Prepending
    Buffer &prepend(const char *string);
    Buffer &prepend(const Buffer &string);
    Buffer &prepend(const char *string, size_t length);
    Buffer &prepend(const Buffer &string, size_t length);
    Buffer &prepend(char c, size_t count);

    // Inserting
    Buffer &insert(const char *string, ssize_t position);
    Buffer &insert(const char *string, size_t length, ssize_t position);
    Buffer &insert(const char *string, size_t fromOffset, size_t length, ssize_t position);
    Buffer &insert(const Buffer &string, ssize_t position);
    Buffer &insert(const Buffer &string, size_t length, ssize_t position);
    Buffer &insert(const Buffer &string, size_t fromOffset, size_t length, ssize_t position);
    Buffer &insert(char c, size_t count, ssize_t position);

    // Removing
    Buffer &truncate(size_t newLength);
    Buffer &erase(size_t from, size_t length);

    Buffer &removeFirst(const Buffer &string);
    Buffer &removeLast(const Buffer &string);
    Buffer &removeAll(const Buffer &string);

    Buffer &removeFirst(const char *string);
    Buffer &removeLast(const char *string);
    Buffer &removeAll(const char *string);

    Buffer &removeSet(const char *setOfCharsToRemove);

    Buffer &moveInto(Buffer &into, size_t from, size_t length);
    void moveInto(char *into, size_t from, size_t length);

    // Compare functions
    bool operator<(const Buffer &string) const;
    bool operator<=(const Buffer &string) const;
    bool operator==(const Buffer &string) const;
    bool operator>=(const Buffer &string) const;
    bool operator>(const Buffer &string) const;
    bool operator!=(const Buffer &string) const;

    bool operator<(const char *string) const;
    bool operator<=(const char *string) const;
    bool operator==(const char *string) const;
    bool operator>=(const char *string) const;
    bool operator>(const char *string) const;
    bool operator!=(const char *string) const;

    /**
     *  Compares two strings.
     */
    int compare(const Buffer &string) const;
    /**
     *  Compares a string against a C string.
     */
    int compare(const char *string) const;
    /**
     *  Compares two strings, at most the first len characters.
     */
    int compare(const Buffer &string, size_t len) const;
    /**
     *  Compares a string against a C string.
     */
    int compare(const char *string, size_t len) const;
    /**
     *  Compares two C strings, case insensitive.
     */
    int compareI(const Buffer &string) const;
    /**
     *  Compares two C strings, case insensitive.
     */
    int compareI(const char *string) const;
    /**
     *  Compares two C strings, case insensitive, at most the first len characters.
     */
    int compareI(const Buffer &string, size_t len) const;
    /**
     *  Compares two C strings, case insensitive, at most the first len characters.
     */
    int compareI(const char *string, size_t len) const;

    // Searching
    size_t findFirst(const Buffer &string) const;
    size_t findFirst(const char *string) const;
    size_t findFirst(const Buffer &string, size_t fromOffset) const;
    size_t findFirst(const char *string, size_t fromOffset) const;
    size_t findFirst(char c) const;
    size_t findFirst(char c, size_t fromOffset) const;

    size_t findLast(const Buffer &string) const;
    size_t findLast(const char *string) const;
    size_t findLast(const Buffer &string, size_t beforeOffset) const;
    size_t findLast(const char *string, size_t beforeOffset) const;
    size_t findLast(char c) const;
    size_t findLast(char c, size_t beforeOffset) const;

    size_t findFirstI(const Buffer &string) const;
    size_t findFirstI(const char *string) const;
    size_t findFirstI(const Buffer &string, size_t fromOffset) const;
    size_t findFirstI(const char *string, size_t fromOffset) const;

    size_t findLastI(const Buffer &string) const;
    size_t findLastI(const char *string) const;
    size_t findLastI(const Buffer &string, size_t beforeOffset) const;
    size_t findLastI(const char *string, size_t beforeOffset) const;

    // Replacing
    Buffer &replaceFirst(char replaceThis, char withThis);
    Buffer &replaceLast(char replaceThis, char withThis);
    Buffer &replaceAll(char replaceThis, char withThis, size_t fromOffset = 0);
    Buffer &replace(char replaceThis, char withThis, unsigned int maxReplaceCount, size_t fromOffset = 0);
    Buffer &replaceFirst(const char *replaceThis, const char *withThis);
    Buffer &replaceLast(const char *replaceThis, const char *withThis);
    Buffer &replaceAll(const char *replaceThis, const char *withThis, size_t fromOffset = 0);
    Buffer &replace(const char *replaceThis, const char *withThis, unsigned int maxReplaceCount, size_t fromOffset = 0);

    Buffer &replaceFirstI(char replaceThis, char withThis);
    Buffer &replaceLastI(char replaceThis, char withThis);
    Buffer &replaceAllI(char replaceThis, char withThis, size_t fromOffset = 0);
    Buffer &replaceI(char replaceThis, char withThis, unsigned int maxReplaceCount, size_t fromOffset = 0);
    Buffer &replaceFirstI(const char *replaceThis, const char *withThis);
    Buffer &replaceLastI(const char *replaceThis, const char *withThis);
    Buffer &replaceAllI(const char *replaceThis, const char *withThis, size_t fromOffset = 0);
    Buffer &replaceI(const char *replaceThis, const char *withThis, unsigned int maxReplaceCount, size_t fromOffset = 0);

    Buffer &replaceSet(const char *setOfChars, char with);
    Buffer &replaceSet(const char *setOfChars, const char *with);

    // Unchecked char access
    char operator[](size_t index) const;

    char &operator[](size_t index);

    // Checked char access
    char byteAt(size_t index) const;

    // Fast low-level manipulation
    /**
     *  Locks the buffer.  Buffer must be unlocked when done.
     *  (Or the object can be destroyed safely).
     *  @return  A buffer which may be written directly to, up to maxLength chars.
     */
    char *lockBuffer(size_t maxLength);
    /**
     *  @param length  The amount of the locked buffer that was filled.  Default (0) implies
     *      using strlen to determine the length.
     */
    Buffer &unlockBuffer(size_t length = 0);

    // Upercase <-> Lowercase
    Buffer &toLower();
    Buffer &toUpper();

    // Escaping and De-escaping
    Buffer &characterEscape(const char *original, const char *setOfCharsToEscape, char escapeWith);
    Buffer &characterEscape(const char *setOfCharsToEscape, char escapeWith);
    Buffer &characterDeescape(const char *original, char escapeChar);
    Buffer &characterDeescape(char escapeChar);

    // Insert
    Buffer &operator<<(const char *string);
    Buffer &operator<<(const Buffer &string);
    Buffer &operator<<(char c);
    Buffer &operator<<(unsigned int value);
    Buffer &operator<<(int32_t value);
    Buffer &operator<<(uint64_t value);
    Buffer &operator<<(int64_t value);
    // float output hardcodes %.2f style formatting
    Buffer &operator<<(float value);

private:
    class PosVect;

    // Management
    char *_alloc(size_t length, bool adoptReferenceCount = true);
    char *_realloc(size_t length);
    void _init(const char *src, size_t length);
    char *_clone(const char *data, size_t length);
    char *_openAtBy(size_t offset, size_t length);
    char *_shrinkAtBy(size_t offset, size_t length);
    void _detachWith(const char *string, size_t dataLen, size_t totalLen);

    // Data
    void _setLength(size_t length);
    void _doAppend(const char *string, size_t length);
    void _doPrepend(const char *string, size_t length);
    void _doInsert(const char *string, size_t offset, size_t length);

    // Search
    size_t _shortFindAfter(const char *string, size_t len) const;
    size_t _findAfter(const char *string, size_t offset, size_t strlen) const;
    size_t _findAfterI(const char *string, size_t offset, size_t strlen) const;

    size_t _findBefore(const char *string, size_t offset, size_t strlen) const;
    size_t _findBeforeI(const char *string, size_t offset, size_t strlen) const;

    // Escape
    Buffer &_doCharacterEscape(const char *string, const char *setOfCharsToEscape, char escapeChar);
    Buffer &_doCharacterDeescape(const char *string, char escapeChar);

    // Replace
    Buffer &_doReplace(const char *findThis, const char *replaceWith, unsigned int maxReplaceCount,
            size_t fromOffset, bool ignoreCase);
    void _replaceAtPositions(const PosVect *positions, size_t searchLen, const char *with,
            size_t withLen);

    std::atomic<int32_t> &refCount();
    const std::atomic<int32_t> &refCount() const;
    void incRef();
    int32_t decRef();

    bool _isShareable() const;
    void _freePrivateData();
};


// Commutative compare operators
bool operator<(const char *a, const Buffer &b);
bool operator<=(const char *a, const Buffer &b);
bool operator==(const char *a, const Buffer &b);
bool operator>(const char *a, const Buffer &b);
bool operator>=(const char *a, const Buffer &b);
bool operator!=(const char *a, const Buffer &b);


// Non-member compare for sorting, etc.
int compare(const Buffer &a, const Buffer &b);
int compareI(const Buffer &a, const Buffer &b);
int compare(const Buffer *a, const Buffer *b);
int compareI(const Buffer *a, const Buffer *b);


inline size_t Buffer::length() const
{
    return *(((size_t *)m_data) - 1);
}

inline Buffer &Buffer::setTo(const char *string)
{
    return operator=(string);
}

inline char Buffer::operator[](size_t index) const
{
    return m_data[index];
}

inline char &Buffer::operator[](size_t index)
{
    return m_data[index];
}

inline char Buffer::byteAt(size_t index) const
{
    if (index > length())
        return 0;
    return m_data[index];
}

inline Buffer &Buffer::operator+=(const Buffer &string)
{
    _doAppend(string.c_str(), string.length());
    return *this;
}

inline Buffer &Buffer::append(const Buffer &string)
{
    _doAppend(string.c_str(), string.length());
    return *this;
}

inline Buffer &Buffer::append(const char *string)
{
    return operator+=(string);
}

inline bool Buffer::operator==(const Buffer &string) const
{
    return compare(string) == 0;
}

inline bool Buffer::operator<(const Buffer &string) const
{
    return compare(string) < 0;
}

inline bool Buffer::operator<=(const Buffer &string) const
{
    return compare(string) <= 0;
}

inline bool Buffer::operator>=(const Buffer &string) const
{
    return compare(string) >= 0;
}

inline bool Buffer::operator>(const Buffer &string) const
{
    return compare(string) > 0;
}

inline bool Buffer::operator!=(const Buffer &string) const
{
    return compare(string) != 0;
}

inline bool Buffer::operator!=(const char *string) const
{
    return !operator==(string);
}

inline bool operator<(const char *str, const Buffer &string)
{
    return string > str;
}

inline bool operator<=(const char *str, const Buffer &string)
{
    return string >= str;
}

inline bool operator==(const char *str, const Buffer &string)
{
    return string == str;
}

inline bool operator>(const char *str, const Buffer &string)
{
    return string < str;
}

inline bool operator>=(const char *str, const Buffer &string)
{
    return string <= str;
}

inline bool operator!=(const char *str, const Buffer &string)
{
    return string != str;
}

#endif

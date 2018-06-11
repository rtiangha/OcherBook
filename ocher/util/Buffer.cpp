#include "Buffer.h"

#include "doctest.h"

#define __STDC_FORMAT_MACROS
#include <cassert>
#include <cctype>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <new>

// define proper names for case-option of _doReplace()
#define KEEP_CASE false
#define IGNORE_CASE true

// define proper names for count-option of _doReplace()
#define REPLACE_ALL 0x7FFFFFFF

#undef max
#define max(a, b) ((a) > (b) ? (a) : (b))

const size_t kPrivateDataOffset = sizeof(int32_t) + sizeof(size_t);


static inline size_t min_clamp0(size_t num1, size_t num2)
{
    if (num1 < num2)
        return num1 > 0 ? num1 : 0;

    return num2 > 0 ? num2 : 0;
}

static inline const char *safestr(const char *str)
{
    return str ? str : "";
}

static int memncmp(const void *m1, const void *m2, size_t n1, size_t n2)
{
    size_t min = min_clamp0(n1, n2);
    int r = memcmp(m1, m2, min);

    if (r || n1 == n2)
        return r;
    if (n1 < n2)
        return -1;
    return 1;
}

static inline std::atomic<int32_t> &referenceCount(char *privateData)
{
    return *reinterpret_cast<std::atomic<int32_t> *>(privateData - sizeof(size_t) - sizeof(int32_t));
}

inline std::atomic<int32_t> &Buffer::refCount()
{
    return referenceCount(m_data);
}

inline const std::atomic<int32_t> &Buffer::refCount() const
{
    return referenceCount(m_data);
}

inline void Buffer::incRef()
{
    std::atomic_fetch_add_explicit(&refCount(), 1, std::memory_order_relaxed);
}

inline int32_t Buffer::decRef()
{
    return std::atomic_fetch_sub_explicit(&refCount(), 1, std::memory_order_relaxed);
}

static inline void setLength(size_t len, char *privateData)
{
    *(((size_t *)privateData) - 1) = len;
}

inline void Buffer::_setLength(size_t len)
{
    setLength(len, m_data);
}

inline bool Buffer::_isShareable() const
{
    return refCount() >= 0;
}

class Buffer::PosVect {
public:
    ~PosVect()
    {
        free(fBuffer);
    }

    void Add(size_t pos)
    {
        if (fBuffer == nullptr || fSize == fBufferSize) {
            if (fBuffer != nullptr)
                fBufferSize *= 2;

            size_t *newBuffer = nullptr;
            newBuffer = (size_t *)realloc(fBuffer, fBufferSize * sizeof(size_t));
            if (newBuffer == nullptr)
                throw std::bad_alloc();

            fBuffer = newBuffer;
        }

        fBuffer[fSize++] = pos;
    }

    inline size_t ItemAt(size_t index) const
    {
        return fBuffer[index];
    }

    inline size_t CountItems() const
    {
        return fSize;
    }

private:
    size_t fSize = 0;
    size_t fBufferSize = 20;
    size_t *fBuffer = nullptr;
};


Buffer::Buffer()
{
    _init("", 0);
}

Buffer::Buffer(const char *string)
{
    _init(string, strlen(safestr(string)));
}

Buffer::Buffer(const Buffer &string)
{
    // check if source is sharable - if so, share else clone
    if (string._isShareable()) {
        m_data = string.m_data;
        incRef();
    } else {
        _init(string.c_str(), string.length());
    }
}

Buffer::Buffer(const char *string, size_t len)
{
    _init(string, len);
}

Buffer::Buffer(size_t prealloc)
{
    _init(nullptr, prealloc);
}

Buffer::~Buffer()
{
    if (!_isShareable() || decRef() == 0)
        _freePrivateData();
}

Buffer &Buffer::operator=(const Buffer &string)
{
    return setTo(string);
}

Buffer &Buffer::operator=(const char *string)
{
    if (!string)
        clear();
    else if (string != c_str())
        setTo(string, strlen(string));
    return *this;
}

Buffer &Buffer::operator=(char c)
{
    return setTo(c, 1);
}

Buffer &Buffer::setTo(const char *string, size_t len)
{
    _detachWith(string, len, len);
    return *this;
}

Buffer &Buffer::setTo(const Buffer &string)
{
    // we share the information already
    if (m_data == string.m_data)
        return *this;

    bool freeData = true;
    if (_isShareable() && decRef() > 0) {
        // there is still someone who shares our data
        freeData = false;
    }

    if (freeData)
        _freePrivateData();

    // if source is sharable share, otherwise clone
    if (string._isShareable()) {
        m_data = string.m_data;
        incRef();
    } else
        _init(string.c_str(), string.length());
    return *this;
}

Buffer &Buffer::adopt(Buffer &from)
{
    setTo(from);
    from.setTo("");

    return *this;
}

Buffer &Buffer::setTo(const Buffer &string, size_t len)
{
    size_t dataLen = min_clamp0(len, string.length());

    if (m_data != string.m_data) {
        _detachWith(string.c_str(), dataLen, len);
    } else if (length() > len) {
        // need to truncate
        _detachWith("", 0, len);
        memmove(m_data, string.c_str(), dataLen);
    }
    return *this;
}

Buffer &Buffer::adopt(Buffer &from, size_t len)
{
    setTo(from, len);
    from.setTo("");
    return *this;
}

Buffer &Buffer::setTo(char c, size_t count)
{
    _detachWith("", 0, count);
    memset(m_data, c, count);
    return *this;
}

Buffer &Buffer::copyInto(Buffer &into, size_t fromOffset, size_t len) const
{
    if (this != &into)
        into.setTo(m_data + fromOffset, len);
    return into;
}

void Buffer::copyInto(char *into, size_t fromOffset, size_t len) const
{
    if (into) {
        len = min_clamp0(len, length() - fromOffset);
        memcpy(into, m_data + fromOffset, len);
    }
}

Buffer &Buffer::operator+=(const char *string)
{
    size_t len = strlen(string);

    _doAppend(string, len);
    return *this;
}

Buffer &Buffer::operator+=(char c)
{
    _doAppend(&c, 1);
    return *this;
}

Buffer &Buffer::append(const Buffer &string, size_t len)
{
    if (&string != this) {
        len = min_clamp0(len, string.length());
        _doAppend(string.m_data, len);
    }
    return *this;
}

Buffer &Buffer::append(const char *string, size_t len)
{
    _doAppend(string, len);
    return *this;
}

Buffer &Buffer::append(char c, size_t count)
{
    if (count > 0) {
        size_t oldLength = length();
        _detachWith(m_data, oldLength, oldLength + count);
        memset(m_data + oldLength, c, count);
    }
    return *this;
}

Buffer &Buffer::prepend(const char *string)
{
    _doPrepend(string, strlen(string));
    return *this;
}

Buffer &Buffer::prepend(const Buffer &string)
{
    if (&string != this)
        _doPrepend(string.c_str(), string.length());
    return *this;
}

Buffer &Buffer::prepend(const char *string, size_t len)
{
    _doPrepend(string, len);
    return *this;
}

Buffer &Buffer::prepend(const Buffer &string, size_t len)
{
    if (&string != this)
        _doPrepend(string.m_data, min_clamp0(len, string.length()));
    return *this;
}

Buffer &Buffer::prepend(char c, size_t count)
{
    if (count > 0) {
        size_t oldLength = length();
        _detachWith(m_data, oldLength, oldLength + count);
        memmove(m_data + count, m_data, oldLength);
        memset(m_data, c, count);
    }
    return *this;
}

Buffer &Buffer::insert(const char *string, ssize_t position)
{
    if (position < 0 || (size_t)position <= length()) {
        size_t len = strlen(string);
        if (position < 0) {
            size_t skipLen = min_clamp0(-1 * position, len);
            string += skipLen;
            len -= skipLen;
            position = 0;
        } else {
            position = min_clamp0(position, length());
        }
        _doInsert(string, position, len);
    }
    return *this;
}

Buffer &Buffer::insert(const char *string, size_t len, ssize_t position)
{
    if (position < 0 || (size_t)position <= length()) {
        if (position < 0) {
            size_t skipLen = min_clamp0(-1 * position, len);
            string += skipLen;
            len -= skipLen;
            position = 0;
        } else {
            position = min_clamp0(position, length());
        }
        _doInsert(string, position, len);
    }
    return *this;
}

Buffer &Buffer::insert(const char *string, size_t fromOffset, size_t len, ssize_t position)
{
    insert(string + fromOffset, len, position);
    return *this;
}

Buffer &Buffer::insert(const Buffer &string, ssize_t position)
{
    if (&string != this && string.length() > 0)
        insert(string.m_data, position);
    return *this;
}

Buffer &Buffer::insert(const Buffer &string, size_t len, ssize_t position)
{
    if (&string != this && string.length() > 0)
        insert(string.c_str(), len, position);
    return *this;
}

Buffer &Buffer::insert(const Buffer &string, size_t fromOffset, size_t len, ssize_t position)
{
    if (&string != this && string.length() > 0)
        insert(string.c_str() + fromOffset, len, position);
    return *this;
}

Buffer &Buffer::insert(char c, size_t count, ssize_t position)
{
    if (position < 0) {
        count = max(count + position, 0);
        position = 0;
    } else
        position = min_clamp0(position, length());

    if (count > 0) {
        size_t oldLength = length();
        _detachWith(m_data, oldLength, oldLength + count);
        memmove(m_data + position + count, m_data + position, oldLength - position);
        memset(m_data + position, c, count);
    }

    return *this;
}

Buffer &Buffer::truncate(size_t newLength)
{
    if (newLength < length())
        _detachWith(m_data, newLength, newLength);

    return *this;
}

Buffer &Buffer::erase(size_t from, size_t len)
{
    if (len > 0 && from < length())
        _shrinkAtBy(from, min_clamp0(len, (length() - from)));
    return *this;
}

Buffer &Buffer::removeFirst(const Buffer &string)
{
    if (string.length() > 0) {
        size_t pos = _shortFindAfter(string.c_str(), string.length());
        if (pos != NotFound)
            _shrinkAtBy(pos, string.length());
    }
    return *this;
}

Buffer &Buffer::removeLast(const Buffer &string)
{
    size_t pos = _findBefore(string.c_str(), length(), string.length());

    if (pos != NotFound)
        _shrinkAtBy(pos, string.length());

    return *this;
}

Buffer &Buffer::removeAll(const Buffer &string)
{
    if (string.length() == 0 || length() == 0 || findFirst(string) == NotFound)
        return *this;

    fork();

    return _doReplace(string.c_str(), "", REPLACE_ALL, 0, KEEP_CASE);
}

Buffer &Buffer::removeFirst(const char *string)
{
    size_t len = string ? strlen(string) : 0;

    if (len > 0) {
        size_t pos = _shortFindAfter(string, len);
        if (pos != NotFound)
            _shrinkAtBy(pos, len);
    }
    return *this;
}

Buffer &Buffer::removeLast(const char *string)
{
    size_t len = string ? strlen(string) : 0;

    if (len > 0) {
        size_t pos = _findBefore(string, length(), len);
        if (pos != NotFound)
            _shrinkAtBy(pos, len);
    }
    return *this;
}

Buffer &Buffer::removeAll(const char *string)
{
    if (!string || *string == 0 || length() == 0 || findFirst(string) == NotFound)
        return *this;

    fork();

    return _doReplace(string, "", REPLACE_ALL, 0, KEEP_CASE);
}

Buffer &Buffer::removeSet(const char *setOfCharsToRemove)
{
    return replaceSet(setOfCharsToRemove, "");
}

Buffer &Buffer::moveInto(Buffer &into, size_t from, size_t len)
{
    if (len) {
        copyInto(into, from, len);
        erase(from, len);
    }
    return into;
}

void Buffer::moveInto(char *into, size_t from, size_t len)
{
    if (into) {
        copyInto(into, from, len);
        erase(from, len);
    }
}

bool Buffer::operator<(const char *string) const
{
    return strcmp(c_str(), safestr(string)) < 0;
}

bool Buffer::operator<=(const char *string) const
{
    return strcmp(c_str(), safestr(string)) <= 0;
}

bool Buffer::operator==(const char *string) const
{
    return strcmp(c_str(), safestr(string)) == 0;
}

bool Buffer::operator>=(const char *string) const
{
    return strcmp(c_str(), safestr(string)) >= 0;
}

bool Buffer::operator>(const char *string) const
{
    return strcmp(c_str(), safestr(string)) > 0;
}

int Buffer::compare(const Buffer &string) const
{
    return memncmp(c_str(), string.c_str(), length(), string.length());
}

int Buffer::compare(const char *string) const
{
    string = safestr(string);
    return memncmp(c_str(), string, length(), strlen(string));
}

int Buffer::compare(const Buffer &string, size_t len) const
{
    return memncmp(c_str(), string.c_str(), min_clamp0(length(), len), min_clamp0(string.length(), len));
}

int Buffer::compare(const char *string, size_t len) const
{
    string = safestr(string);
    size_t stringLen = strlen(string);
    return memncmp(c_str(), string, min_clamp0(length(), len), min_clamp0(stringLen, len));
}

int Buffer::compareI(const Buffer &string) const
{
    return strcasecmp(c_str(), string.c_str());
}

int Buffer::compareI(const char *string) const
{
    return strcasecmp(c_str(), safestr(string));
}

int Buffer::compareI(const Buffer &string, size_t len) const
{
    return strncasecmp(c_str(), string.c_str(), len);
}

int Buffer::compareI(const char *string, size_t len) const
{
    return strncasecmp(c_str(), safestr(string), len);
}

size_t Buffer::findFirst(const Buffer &string) const
{
    return _shortFindAfter(string.c_str(), string.length());
}

size_t Buffer::findFirst(const char *string) const
{
    assert(string);
    return _shortFindAfter(string, strlen(string));
}

size_t Buffer::findFirst(const Buffer &string, size_t fromOffset) const
{
    return _findAfter(string.c_str(), min_clamp0(fromOffset, length()), string.length());
}

size_t Buffer::findFirst(const char *string, size_t fromOffset) const
{
    assert(string);
    return _findAfter(string, min_clamp0(fromOffset, length()), strlen(safestr(string)));
}

size_t Buffer::findFirst(char c) const
{
    const char *start = c_str();
    const char *end = c_str() + length();

    // Scans the string until we found the
    // character, or we reach the string's start
    while (start != end && *start != c) {
        start++;
    }

    if (start == end)
        return NotFound;

    return start - c_str();
}

size_t Buffer::findFirst(char c, size_t fromOffset) const
{
    const char *start = c_str() + min_clamp0(fromOffset, length());
    const char *end = c_str() + length();

    // Scans the string until we found the
    // character, or we reach the string's start
    while (start < end && *start != c) {
        start++;
    }

    if (start >= end)
        return NotFound;

    return start - c_str();
}

size_t Buffer::findLast(const Buffer &string) const
{
    return _findBefore(string.c_str(), length(), string.length());
}

size_t Buffer::findLast(const char *string) const
{
    assert(string);
    return _findBefore(string, length(), strlen(safestr(string)));
}

size_t Buffer::findLast(const Buffer &string, size_t beforeOffset) const
{
    return _findBefore(string.c_str(), min_clamp0(beforeOffset, length()), string.length());
}

size_t Buffer::findLast(const char *string, size_t beforeOffset) const
{
    assert(string);
    return _findBefore(string, min_clamp0(beforeOffset, length()), strlen(safestr(string)));
}

size_t Buffer::findLast(char c) const
{
    const char *start = c_str();
    const char *end = c_str() + length();

    // Scans the string backwards until we found
    // the character, or we reach the string's start
    while (end != start && *end != c) {
        end--;
    }

    if (end == start && *end != c)
        return NotFound;

    return end - c_str();
}

size_t Buffer::findLast(char c, size_t beforeOffset) const
{
    const char *start = c_str();
    const char *end = c_str() + min_clamp0(beforeOffset, length());

    // Scans the string backwards until we found
    // the character, or we reach the string's start
    while (end > start && *end != c) {
        end--;
    }

    if (end <= start && *end != c)
        return NotFound;

    return end - c_str();
}

size_t Buffer::findFirstI(const Buffer &string) const
{
    return _findAfterI(string.c_str(), 0, string.length());
}

size_t Buffer::findFirstI(const char *string) const
{
    assert(string);
    return _findAfterI(string, 0, strlen(safestr(string)));
}

size_t Buffer::findFirstI(const Buffer &string, size_t fromOffset) const
{
    return _findAfterI(string.c_str(), min_clamp0(fromOffset, length()), string.length());
}

size_t Buffer::findFirstI(const char *string, size_t fromOffset) const
{
    assert(string);
    return _findAfterI(string, min_clamp0(fromOffset, length()), strlen(safestr(string)));
}

size_t Buffer::findLastI(const Buffer &string) const
{
    return _findBeforeI(string.c_str(), length(), string.length());
}

size_t Buffer::findLastI(const char *string) const
{
    assert(string);
    return _findBeforeI(string, length(), strlen(safestr(string)));
}

size_t Buffer::findLastI(const Buffer &string, size_t beforeOffset) const
{
    return _findBeforeI(string.c_str(), min_clamp0(beforeOffset, length()), string.length());
}

size_t Buffer::findLastI(const char *string, size_t beforeOffset) const
{
    assert(string);
    return _findBeforeI(string, min_clamp0(beforeOffset, length()), strlen(safestr(string)));
}

Buffer &Buffer::replaceFirst(char replaceThis, char withThis)
{
    size_t pos = findFirst(replaceThis);

    if (pos != NotFound) {
        fork();
        m_data[pos] = withThis;
    }
    return *this;
}

Buffer &Buffer::replaceLast(char replaceThis, char withThis)
{
    size_t pos = findLast(replaceThis);

    if (pos != NotFound) {
        fork();
        m_data[pos] = withThis;
    }
    return *this;
}

Buffer &Buffer::replaceAll(char replaceThis, char withThis, size_t fromOffset)
{
    fromOffset = min_clamp0(fromOffset, length());
    size_t pos = findFirst(replaceThis, fromOffset);

    // detach and set first match
    if (pos != NotFound) {
        fork();
        m_data[pos] = withThis;
        while (true) {
            pos = findFirst(replaceThis, pos);
            if (pos == NotFound)
                break;
            m_data[pos] = withThis;
        }
    }
    return *this;
}

Buffer &Buffer::replace(char replaceThis, char withThis, unsigned int maxReplaceCount,
        size_t fromOffset)
{
    fromOffset = min_clamp0(fromOffset, length());
    size_t pos = findFirst(replaceThis, fromOffset);

    if (maxReplaceCount > 0 && pos != NotFound) {
        fork();
        maxReplaceCount--;
        m_data[pos] = withThis;
        for (; maxReplaceCount > 0; maxReplaceCount--) {
            pos = findFirst(replaceThis, pos);
            if (pos == NotFound)
                break;
            m_data[pos] = withThis;
        }
    }
    return *this;
}

Buffer &Buffer::replaceFirst(const char *replaceThis, const char *withThis)
{
    if (!replaceThis || !withThis || findFirst(replaceThis) == NotFound)
        return *this;

    fork();

    return _doReplace(replaceThis, withThis, 1, 0, KEEP_CASE);
}

Buffer &Buffer::replaceLast(const char *replaceThis, const char *withThis)
{
    if (!replaceThis || !withThis)
        return *this;

    size_t replaceThisLength = strlen(replaceThis);
    size_t pos = _findBefore(replaceThis, length(), replaceThisLength);

    if (pos != NotFound) {
        size_t withThisLength =  strlen(withThis);
        ssize_t difference = withThisLength - replaceThisLength;

        if (difference > 0) {
            if (!_openAtBy(pos, difference))
                return *this;
        } else if (difference < 0) {
            if (!_shrinkAtBy(pos, -difference))
                return *this;
        } else {
            fork();
        }
        memcpy(m_data + pos, withThis, withThisLength);
    }

    return *this;
}

Buffer &Buffer::replaceAll(const char *replaceThis, const char *withThis,
        size_t fromOffset)
{
    if (!replaceThis || !withThis || findFirst(replaceThis) == NotFound)
        return *this;

    fork();

    return _doReplace(replaceThis, withThis, REPLACE_ALL,
            min_clamp0(fromOffset, length()), KEEP_CASE);
}

Buffer &Buffer::replace(const char *replaceThis, const char *withThis, unsigned int maxReplaceCount,
        size_t fromOffset)
{
    if (!replaceThis || !withThis || maxReplaceCount == 0 || findFirst(replaceThis) == NotFound)
        return *this;

    fork();

    return _doReplace(replaceThis, withThis, maxReplaceCount,
            min_clamp0(fromOffset, length()), KEEP_CASE);
}

Buffer &Buffer::replaceFirstI(char replaceThis, char withThis)
{
    char tmp[2] = { replaceThis, '\0' };

    size_t pos = _findAfterI(tmp, 0, 1);

    if (pos != NotFound) {
        fork();
        m_data[pos] = withThis;
    }
    return *this;
}

Buffer &Buffer::replaceLastI(char replaceThis, char withThis)
{
    char tmp[2] = { replaceThis, '\0' };

    size_t pos = _findBeforeI(tmp, length(), 1);

    if (pos != NotFound) {
        fork();
        m_data[pos] = withThis;
    }
    return *this;
}

Buffer &Buffer::replaceAllI(char replaceThis, char withThis, size_t fromOffset)
{
    char tmp[2] = { replaceThis, '\0' };

    fromOffset = min_clamp0(fromOffset, length());
    size_t pos = _findAfterI(tmp, fromOffset, 1);

    if (pos != NotFound) {
        fork();
        m_data[pos] = withThis;
        while (true) {
            pos = _findAfterI(tmp, pos, 1);
            if (pos == NotFound)
                break;
            m_data[pos] = withThis;
        }
    }
    return *this;
}

Buffer &Buffer::replaceI(char replaceThis, char withThis, unsigned int maxReplaceCount,
        size_t fromOffset)
{
    char tmp[2] = { replaceThis, '\0' };

    fromOffset = min_clamp0(fromOffset, length());
    size_t pos = _findAfterI(tmp, fromOffset, 1);

    if (maxReplaceCount > 0 && pos != NotFound) {
        fork();
        m_data[pos] = withThis;
        maxReplaceCount--;
        for (; maxReplaceCount > 0; maxReplaceCount--) {
            pos = _findAfterI(tmp, pos, 1);
            if (pos == NotFound)
                break;
            m_data[pos] = withThis;
        }
    }

    return *this;
}

Buffer &Buffer::replaceFirstI(const char *replaceThis, const char *withThis)
{
    if (!replaceThis || !withThis || findFirstI(replaceThis) == NotFound)
        return *this;

    fork();
    return _doReplace(replaceThis, withThis, 1, 0, IGNORE_CASE);
}

Buffer &Buffer::replaceLastI(const char *replaceThis, const char *withThis)
{
    if (!replaceThis || !withThis)
        return *this;

    size_t replaceThisLength = strlen(replaceThis);
    size_t pos = _findBeforeI(replaceThis, length(), replaceThisLength);

    if (pos != NotFound) {
        size_t withThisLength = strlen(withThis);
        ssize_t difference = withThisLength - replaceThisLength;

        if (difference > 0) {
            if (!_openAtBy(pos, difference))
                return *this;
        } else if (difference < 0) {
            if (!_shrinkAtBy(pos, -difference))
                return *this;
        } else {
            fork();
        }
        memcpy(m_data + pos, withThis, withThisLength);
    }

    return *this;
}

Buffer &Buffer::replaceAllI(const char *replaceThis, const char *withThis,
        size_t fromOffset)
{
    if (!replaceThis || !withThis || findFirstI(replaceThis) == NotFound)
        return *this;

    fork();

    return _doReplace(replaceThis, withThis, REPLACE_ALL,
            min_clamp0(fromOffset, length()), IGNORE_CASE);
}

Buffer &Buffer::replaceI(const char *replaceThis, const char *withThis,
        unsigned int maxReplaceCount, size_t fromOffset)
{
    if (!replaceThis || !withThis || maxReplaceCount == 0 || findFirst(replaceThis) == NotFound)
        return *this;

    fork();

    return _doReplace(replaceThis, withThis, maxReplaceCount,
            min_clamp0(fromOffset, length()), IGNORE_CASE);
}

Buffer &Buffer::replaceSet(const char *setOfChars, char with)
{
    if (!setOfChars || strcspn(m_data, setOfChars) >= length())
        return *this;

    fork();

    size_t offset = 0;
    size_t len = length();
    for (size_t pos;; ) {
        pos = strcspn(m_data + offset, setOfChars);

        offset += pos;
        if (offset >= len)
            break;

        m_data[offset] = with;
        offset++;
    }

    return *this;
}

Buffer &Buffer::replaceSet(const char *setOfChars, const char *with)
{
    if (!setOfChars || !with || strcspn(m_data, setOfChars) >= length())
        return *this;

    // delegate simple case
    size_t withLen = strlen(with);
    if (withLen == 1)
        return replaceSet(setOfChars, *with);

    fork();

    size_t pos = 0;
    size_t searchLen = 1;
    size_t len = length();

    PosVect positions;
    for (size_t offset = 0; offset < len; offset += (pos + searchLen)) {
        pos = strcspn(m_data + offset, setOfChars);
        if (pos + offset >= len)
            break;
        positions.Add(offset + pos);
    }

    _replaceAtPositions(&positions, searchLen, with, withLen);
    return *this;
}

char *Buffer::lockBuffer(size_t maxLength)
{
    _detachWith(m_data, length(), maxLength);
    refCount() = -1;  // unsharable
    return m_data;
}

Buffer &Buffer::unlockBuffer(size_t len)
{
    if (len > 0) {
        len = min_clamp0(len, length());
    } else {
        len = strlen(m_data);
    }

    // is unsharable, so was and remains only mine
    _realloc(len);
    m_data[len] = '\0';
    refCount() = 1;  // mark shareable again

    return *this;
}

Buffer &Buffer::toLower()
{
    size_t len = length();

    if (len > 0) {
        fork();
        for (size_t count = 0; count < len; count++) {
            m_data[count] = tolower(m_data[count]);
        }
    }
    return *this;
}

Buffer &Buffer::toUpper()
{
    size_t len = length();

    if (len > 0) {
        fork();
        for (size_t count = 0; count < len; count++) {
            m_data[count] = toupper(m_data[count]);
        }
    }
    return *this;
}

Buffer &Buffer::characterEscape(const char *original,
        const char *setOfCharsToEscape, char escapeWith)
{
    if (setOfCharsToEscape)
        _doCharacterEscape(original, setOfCharsToEscape, escapeWith);
    return *this;
}

Buffer &Buffer::characterEscape(const char *setOfCharsToEscape, char escapeWith)
{
    if (setOfCharsToEscape && length() > 0)
        _doCharacterEscape(m_data, setOfCharsToEscape, escapeWith);
    return *this;
}

Buffer &Buffer::characterDeescape(const char *original, char escapeChar)
{
    return _doCharacterDeescape(original, escapeChar);
}

Buffer &Buffer::characterDeescape(char escapeChar)
{
    if (length() > 0)
        _doCharacterDeescape(m_data, escapeChar);
    return *this;
}

Buffer &Buffer::operator<<(const char *string)
{
    size_t len = strlen(string);

    _doAppend(string, len);
    return *this;
}

Buffer &Buffer::operator<<(const Buffer &string)
{
    _doAppend(string.c_str(), string.length());
    return *this;
}

Buffer &Buffer::operator<<(char c)
{
    _doAppend(&c, 1);
    return *this;
}

Buffer &Buffer::operator<<(unsigned int i)
{
    char num[32];
    size_t len = snprintf(num, sizeof(num), "%u", i);

    _doAppend(num, len);
    return *this;
}

Buffer &Buffer::operator<<(int32_t i)
{
    char num[32];
    size_t len = snprintf(num, sizeof(num), "%d", i);

    _doAppend(num, len);
    return *this;
}

Buffer &Buffer::operator<<(uint64_t i)
{
    char num[64];
    size_t len = snprintf(num, sizeof(num), "%" PRIu64, i);

    _doAppend(num, len);
    return *this;
}

Buffer &Buffer::operator<<(int64_t i)
{
    char num[64];
    size_t len = snprintf(num, sizeof(num), "%" PRId64, i);

    _doAppend(num, len);
    return *this;
}

Buffer &Buffer::operator<<(float f)
{
    char num[64];
    size_t len = snprintf(num, sizeof(num), "%.2f", f);

    _doAppend(num, len);
    return *this;
}

void Buffer::fork()
{
    if (refCount() > 1) {
        // Note:  Ordering is significant due to possible exceptions
        char *newData = _clone(m_data, length());
        if (decRef() == 0)
            _freePrivateData();
        m_data = newData;
    }
}

char *Buffer::_alloc(size_t len, bool adoptReferenceCount)
{
    char *newData = (char *)malloc(len + kPrivateDataOffset + 1);

    if (!newData)
        throw std::bad_alloc();

    newData += kPrivateDataOffset;
    newData[len] = '\0';

    int32_t n;
    if (adoptReferenceCount)
        n = refCount();
    else
        n = 1;
    referenceCount(newData) = n;
    setLength(len, newData);

    return newData;
}

char *Buffer::_realloc(size_t len)
{
    if (len == length())
        return m_data;

    int32_t oldReferenceCount = refCount();
    assert(oldReferenceCount == -1 || oldReferenceCount == 1);  // Must be unsharable or unshared
    char *dataPtr = m_data - kPrivateDataOffset;

    dataPtr = (char *)realloc(dataPtr, len + kPrivateDataOffset + 1);
    if (!dataPtr)
        throw std::bad_alloc();
    dataPtr += kPrivateDataOffset;

    m_data = dataPtr;
    m_data[len] = '\0';

    _setLength(len);
    refCount() = oldReferenceCount;
    return dataPtr;
}

void Buffer::_init(const char *src, size_t len)
{
    m_data = _clone(src, len);
}

char *Buffer::_clone(const char *data, size_t len)
{
    char *newData = _alloc(len, false);

    if (data)
        memcpy(newData, data, len);
    return newData;
}

char *Buffer::_openAtBy(size_t offset, size_t len)
{
    size_t oldLength = length();

    fork();
    memmove(m_data + offset + len, m_data + offset, oldLength - offset);
    return _realloc(oldLength + len);
}

char *Buffer::_shrinkAtBy(size_t offset, size_t len)
{
    size_t oldLength = length();

    fork();
    memmove(m_data + offset, m_data + offset + len, oldLength - offset - len);
    return _realloc(oldLength - len);
}

void Buffer::_detachWith(const char *data, size_t dataLen, size_t totalLen)
{
    if (refCount() > 1) {
        // Note:  Ordering is significant due to possible exceptions
        char *newData = _alloc(totalLen, false);
        if (data) {
            dataLen = min_clamp0(dataLen, totalLen);
            memcpy(newData, data, dataLen);
        }
        if (decRef() == 0)
            _freePrivateData();
        m_data = newData;
    } else {
        bool self = (data == m_data);
        _realloc(totalLen);
        if (!self && data) {
            dataLen = min_clamp0(dataLen, totalLen);
            memcpy(m_data, data, dataLen);
        }
    }
}

void Buffer::_freePrivateData()
{
    free(m_data - kPrivateDataOffset);
    m_data = nullptr;
}

void Buffer::_doAppend(const char *string, size_t len)
{
    if (len > 0) {
        size_t oldLength = length();
        _detachWith(m_data, oldLength, oldLength + len);
        memcpy(m_data + oldLength, string, len);
    }
}

void Buffer::_doPrepend(const char *string, size_t len)
{
    if (len > 0) {
        size_t oldLength = length();
        _detachWith(m_data, oldLength, oldLength + len);
        memmove(m_data + len, m_data, oldLength);
        memcpy(m_data, string, len);
    }
}

void Buffer::_doInsert(const char *string, size_t offset, size_t len)
{
    if (len > 0) {
        size_t oldLength = length();
        _detachWith(m_data, oldLength, oldLength + len);
        memmove(m_data + offset + len, m_data + offset, oldLength - offset);
        memcpy(m_data + offset, string, len);
    }
}

size_t Buffer::_shortFindAfter(const char *string, size_t /*len*/) const
{
    // TODO  honor len
    const char *ptr = strstr(c_str(), string);

    if (ptr != nullptr)
        return ptr - c_str();

    return NotFound;
}

size_t Buffer::_findAfter(const char *string, size_t offset, size_t /*strlen*/) const
{
    // TODO  honor len
    const char *ptr = strstr(c_str() + offset, string);

    if (ptr != nullptr)
        return ptr - c_str();

    return NotFound;
}

size_t Buffer::_findAfterI(const char *string, size_t offset, size_t /*strlen*/) const
{
    // TODO  honor len
    const char *ptr = strcasestr(c_str() + offset, string);

    if (ptr != nullptr)
        return ptr - c_str();

    return NotFound;
}

size_t Buffer::_findBefore(const char *string, size_t offset, size_t strlen) const
{
    const char *ptr = m_data + offset - strlen;

    while (ptr >= m_data) {
        if (!memcmp(ptr, string, strlen))
            return ptr - m_data;
        ptr--;
    }
    return NotFound;
}

size_t Buffer::_findBeforeI(const char *string, size_t offset, size_t strlen) const
{
    char *ptr1 = m_data + offset - strlen;

    while (ptr1 >= m_data) {
        if (!strncasecmp(ptr1, string, strlen))
            return ptr1 - m_data;
        ptr1--;
    }
    return NotFound;
}

Buffer &Buffer::_doCharacterEscape(const char *string, const char *setOfCharsToEscape,
        char escapeChar)
{
    size_t len = strlen(safestr(string));

    _detachWith(string, len, len);

    len = length();
    memcpy(m_data, string, len);

    PosVect positions;
    size_t pos = 0;
    for (size_t offset = 0; offset < len; offset += pos + 1) {
        if ((pos = strcspn(m_data + offset, setOfCharsToEscape))
            < len - offset) {
            positions.Add(offset + pos);
        }
    }

    size_t count = positions.CountItems();
    size_t newLength = len + count;
    if (!newLength) {
        _realloc(0);
        return *this;
    }

    char *newData = _alloc(newLength);
    char *oldString = m_data;
    char *newString = newData;
    size_t lastPos = 0;

    for (size_t i = 0; i < count; ++i) {
        pos = positions.ItemAt(i);
        len = pos - lastPos;
        if (len > 0) {
            memcpy(newString, oldString, len);
            oldString += len;
            newString += len;
        }
        *newString++ = escapeChar;
        *newString++ = *oldString++;
        lastPos = pos + 1;
    }

    len = length() + 1 - lastPos;
    if (len > 0)
        memcpy(newString, oldString, len);

    _freePrivateData();
    m_data = newData;
    return *this;
}

Buffer &Buffer::_doCharacterDeescape(const char *string, char escapeChar)
{
    size_t len = strlen(safestr(string));

    _detachWith(string, len, len);

    memcpy(m_data, string, length());
    const char escape[2] = { escapeChar, '\0' };
    return _doReplace(escape, "", REPLACE_ALL, 0, KEEP_CASE);
}

Buffer &Buffer::_doReplace(const char *findThis, const char *replaceWith,
        unsigned int maxReplaceCount, size_t fromOffset, bool ignoreCase)
{
    if (findThis == nullptr || maxReplaceCount == 0 || fromOffset >= length())
        return *this;

    typedef size_t (Buffer::*TFindMethod)(const char *, size_t, size_t) const;
    TFindMethod findMethod = ignoreCase ? &Buffer::_findAfterI : &Buffer::_findAfter;
    size_t findLen = strlen(findThis);

    if (!replaceWith)
        replaceWith = "";

    size_t replaceLen = strlen(replaceWith);
    size_t lastSrcPos = fromOffset;
    PosVect positions;
    for (size_t srcPos = 0; maxReplaceCount > 0
         && (srcPos = (this->*findMethod)(findThis, lastSrcPos, findLen)) != NotFound;
         maxReplaceCount--) {
        positions.Add(srcPos);
        lastSrcPos = srcPos + findLen;
    }
    _replaceAtPositions(&positions, findLen, replaceWith, replaceLen);
    return *this;
}

void Buffer::_replaceAtPositions(const PosVect *positions, size_t searchLength,
        const char *with, size_t withLength)
{
    size_t len = length();
    size_t count = positions->CountItems();
    size_t newLength = len + count * (withLength - searchLength);

    if (!newLength) {
        _realloc(0);
        return;
    }

    char *newData = _alloc(newLength);
    char *oldString = m_data;
    char *newString = newData;
    size_t lastPos = 0;

    for (size_t i = 0; i < count; ++i) {
        size_t pos = positions->ItemAt(i);
        len = pos - lastPos;
        if (len > 0) {
            memcpy(newString, oldString, len);
            oldString += len;
            newString += len;
        }
        memcpy(newString, with, withLength);
        oldString += searchLength;
        newString += withLength;
        lastPos = pos + searchLength;
    }

    len = length() + 1 - lastPos;
    if (len > 0)
        memcpy(newString, oldString, len);

    _freePrivateData();
    m_data = newData;
}

int compare(const Buffer &string1, const Buffer &string2)
{
    return strcmp(string1.c_str(), string2.c_str());
}

int compareI(const Buffer &string1, const Buffer &string2)
{
    return strcasecmp(string1.c_str(), string2.c_str());
}

int compare(const Buffer *string1, const Buffer *string2)
{
    return strcmp(string1->c_str(), string2->c_str());
}

int compareI(const Buffer *string1, const Buffer *string2)
{
    return strcasecmp(string1->c_str(), string2->c_str());
}

TEST_CASE("Buffer constructors") {
    char const *source = "Hello World!";

    SUBCASE("Trivial") {
        Buffer a;

        CHECK(a.length() == 0U);
    }

    SUBCASE("C string") {
        Buffer a(source);

        CHECK(strlen(source) == (size_t)a.length());
        CHECK(a.c_str() != source);  // Should have copied
        CHECK(a == source);
    }

    SUBCASE("Truncated C string") {
        Buffer a(source, 5);

        CHECK(5U == a.length());
        CHECK(a.c_str() != source);  // Should have copied
        CHECK(strncmp(a.c_str(), source, 5) == 0);
    }
}

TEST_CASE("Buffer utf8") {
    // 4 characters:
    //   ampersand     1 byte
    //   cyrillic zhe  2 bytes
    //   chinese       3 bytes
    //   symbol        4 bytes
    Buffer a("&\xd0\x96\xe4\xb8\xad\xf0\x90\x8d\x86");

    CHECK(10U == a.length());
}

TEST_CASE("Buffer copyOnWrite") {
    Buffer a("Hello World");
    Buffer b(a);

    b.append(" Domination");
    CHECK(a == Buffer("Hello World"));
    CHECK(b == Buffer("Hello World Domination"));
}

TEST_CASE("Buffer c_str") {
    const char a[] = "multi\nline\nstring\n";
    Buffer b(a);

    CHECK(memcmp(b.c_str(), a, sizeof(a)) == 0);
}

TEST_CASE("Buffer copy") {
    Buffer *s2;
    Buffer *s4;
    Buffer s5;
    {
        Buffer s1("test123");         // Constructor, on stack
        s2 = new Buffer("test456");   // Constructor, on heap
        Buffer s3(s1);                // Copy constructor, on stack
        s4 = new Buffer(s3);          // Copy constructor, on heap
        s5 = s3;                      // Assignment, to stack value which is going away
    }

    delete s2;
    delete s4;
    // Should not crash when s5 gets destructed (was shared with now-defunct s3)
    // Should have no leaks
}

TEST_CASE("Buffer binary") {
    const size_t len = 16;
    const char data[len] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18,
        0x19, 0x00, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    Buffer s1(data, len);

    CHECK(s1.length() == len);
    CHECK(memcmp(s1.c_str(), data, len) == 0);
    // Copies should be exact -- not somehow coerced to a C string
    Buffer s2(s1);
    CHECK(s2.length() == len);
    CHECK(memcmp(s2.c_str(), data, len) == 0);
}

TEST_CASE("Buffer compare") {
    Buffer s1("testing");
    Buffer s2("testing");
    Buffer s3("Testing");
    Buffer s4("tester");
    Buffer s5("test");

    CHECK(s1 == s2);
    CHECK(s1 != s3);
    CHECK(s1.compare(s2) == 0);
    CHECK(s1.compare(s2.c_str()) == 0);
    CHECK(s1.compare(s3) != 0);
    CHECK(s1.compareI(s2) == 0);
    CHECK(s1.compareI(s3) == 0);
    CHECK(s1 > s5);
    CHECK(s1.compare(s5) > 0);
    CHECK(s1.compare(s5.c_str()) > 0);
    CHECK(s5 < s1);
    CHECK(s5.compare(s1) < 0);
    CHECK(s5.compare(s1.c_str()) < 0);
    CHECK(s1.compare(s4, 4) == 0);
    CHECK(s3.compareI(s4, 4) == 0);
    CHECK(s1.compare(s4, 5) != 0);
    CHECK(s3.compareI(s4, 5) != 0);
}

TEST_CASE("Buffer binarycompare") {
    Buffer s1("te\0ting", 7);
    Buffer s2("te\0ting", 7);
    Buffer s3("Te\0ting", 7);
    Buffer s4("te\0ter", 6);
    Buffer s5("te\0t", 4);

    CHECK(s1 == s2);
    CHECK(s1 != s3);
    CHECK(s1.compare(s2) == 0);
    CHECK(s1.compare(s2.c_str()) > 0);
    CHECK(s1.compare(s3) != 0);
    CHECK(s1.compareI(s2) == 0);
    CHECK(s1.compareI(s3) == 0);
    CHECK(s1 > s5);
    CHECK(s1.compare(s5) > 0);
    CHECK(s1.compare(s5.c_str()) > 0);
    CHECK(s5 < s1);
    CHECK(s5.compare(s1) < 0);
    CHECK(s5.compare(s1.c_str()) > 0);
    CHECK(s1.compare(s4, 4) == 0);
    CHECK(s3.compareI(s4, 4) == 0);
    CHECK(s1.compare(s4, 5) != 0);
    CHECK(s3.compareI(s4, 5) == 0);  // Case implies all treated as C string
}

TEST_CASE("Buffer setToTrunc") {
    Buffer s;

    s.setTo("testing");
    CHECK(s == Buffer("testing"));
    s.setTo(s, 4);
    CHECK(s == Buffer("test"));
}

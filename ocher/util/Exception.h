#ifndef UTIL_EXCEPTION_H
#define UTIL_EXCEPTION_H

/**
 *  @file Common exceptions.
 *
 *  @todo  better state than a string.  e.g., include file
 */

#include <exception>

class Exception : public std::exception {
public:
    Exception(char const *_what = "") throw() :
        m_what(_what)
    {
    }
    virtual char const *what() const throw()
    {
        return m_what;
    }

private:
    char const *m_what;
//        Exception(Exception const&);  ///< Unimplemented; disallowed
//        Exception& operator=(Exception const&);  ///< Unimplemented; disallowed
};

class IOException : public Exception {
public:
    IOException(char const *_what = "") throw() :
        Exception(_what),
        fn(0),
        err(0)
    {
    }
    IOException(char const *_fn, int e) throw() :
        Exception("IOException"),
        fn(_fn),
        err(e)
    {
    }
    const char *fn;
    int err;
};

class BufferOverflowException : public Exception {
public:
    BufferOverflowException(char const *_what = "") throw() :
        Exception(_what)
    {
    }
};

class IllegalArgumentException : public Exception {
public:
    IllegalArgumentException(char const *_what = "") throw() :
        Exception(_what)
    {
    }
};

class IllegalFormatException : public Exception {
public:
    IllegalFormatException(char const *_what = "") throw() :
        Exception(_what)
    {
    }
};

#endif

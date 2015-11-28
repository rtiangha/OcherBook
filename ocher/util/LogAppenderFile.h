#ifndef LIBCLC_LOG_APPENDER_FILE_H
#define LIBCLC_LOG_APPENDER_FILE_H

#include "util/File.h"
#include "util/LogAppenders.h"

/**
 *  Appender that appends to File objects, with optional threading for guaranteed
 *  non-blocking operation.
 */
class LogAppenderFile : public LogAppender {
public:
    /**
     *  @param filename  The filename to log to.  Relative paths will be interpreted relative to
     *      the standard logging directory.
     */
    LogAppenderFile(const char *filename);
    ~LogAppenderFile();
    void append(std::string const &s);

protected:
    File m_file;
};

#endif

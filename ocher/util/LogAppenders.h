#ifndef UTIL_LOG_APPENDERS_H
#define UTIL_LOG_APPENDERS_H

#include "Logger.h"

#include <stdio.h>
#include <string>

class Logger;


/**
 *  Appender that discards messages.
 */
class LogAppenderNull : public LogAppender {
public:
    void append(std::string const &)
    {
    }
};


#if 0
/**
 *  Appender that sends the messages to the debugger (if any).
 */
class LogAppenderDebugger : public LogAppender {
public:
    void append(std::string const &s)
    {
        Debugger::printf(s);
    }
};
#endif


/**
 *  Appender that appends to C-style FILE objects, such as stdout.
 */
class LogAppenderCFile : public LogAppender {
public:
    LogAppenderCFile(FILE *f) :
        m_f(f)
    {
    }

    void append(std::string const &s)
    {
        fprintf(m_f, "%s", s.c_str());
    }

protected:
    FILE *m_f;
};

#endif

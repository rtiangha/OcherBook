#ifndef UTIL_LOG_APPENDERS_H
#define UTIL_LOG_APPENDERS_H

#include "Logger.h"

#include <cstdio>
#include <string>

class Logger;


/**
 *  Appender that discards messages.
 */
class LogAppenderNull : public LogAppender {
public:
    void append(const std::string&) override
    {
    }
};


#if 0
/**
 *  Appender that sends the messages to the debugger (if any).
 */
class LogAppenderDebugger : public LogAppender {
public:
    void append(const std::string& s) override
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
    LogAppenderCFile(FILE* f) :
        m_f(f)
    {
    }

    void append(const std::string& s) override
    {
        fputs(s.c_str(), m_f);
    }

protected:
    FILE* m_f;
};

#endif

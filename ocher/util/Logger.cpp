#include "util/LogAppenders.h"
#include "util/Logger.h"
#include "util/StrUtil.h"

#include <memory>
#include <string>
#include <string.h>

using namespace std;

Loggers Log::loggers;


Loggers::Loggers()
{
    m_init = true;
    setRoot();
}

Loggers::~Loggers()
{
    clear();
    m_init = false;
}

void Loggers::clearAppenderUnchecked(LogAppender *logAppender)
{
    for (auto &logger : m_loggers) {
        logger.second->clearAppender(logAppender);
    }
}

void Loggers::clear()
{
    if (m_init) {
        lock_guard<mutex> lock(m_lock);

        // Break ties to all LogAppenders, so that destruction order of statics does not matter.
        clearAppenderUnchecked(0);
        m_loggers.clear();
    }
}

void Loggers::clearAppender(LogAppender *logAppender)
{
    if (m_init) {
        lock_guard<mutex> lock(m_lock);

        clearAppenderUnchecked(logAppender);
    }
}

void Loggers::setRoot()
{
    // Calling setRoot from another static is bad because I can't guarantee ordering.  Asserting
    // because I can't honor the contract.  Just don't do it.
    assert(m_init);
    if (m_init) {
        Logger *root = new Logger(this, 0, "", 0);
        root->setLevel(Log::Warn);

        {
            lock_guard<mutex> lock(m_lock);
            m_loggers[root->getName()] = root;
        }
    }
}

Logger *Loggers::get(const char *name)
{
    if (!m_init)
        return (Logger *)0;

    m_lock.lock();
    Logger *logger = m_loggers[name];
    m_lock.unlock();

    if (!logger) {
        unsigned int nameLen = strlen(name);
        const char *end;
        unsigned int searchOffset = 0;
        // Root logger should have been created in setRoot.
        assert(nameLen > 0);

        lock_guard<mutex> lock(m_lock);
        Logger *parent = m_loggers[""];
        assert(parent);
        do {
            unsigned int subnameLen;
            end = strchr(name + searchOffset, '.');
            if (end) {
                subnameLen = end - name;
                searchOffset = subnameLen + 1;
            } else {
                subnameLen = nameLen;
            }
            std::string subname(name, subnameLen);
            logger = m_loggers[subname];
            if (!logger) {
                logger = new Logger(this, parent, subname);
                m_loggers[subname] = logger;
            }
            parent = logger;
        } while (end);
    }
    return logger;
}

std::string getLogDirectory()
{
    // TODO
    return "/tmp";
}

void Log::reset()
{
    loggers.clear();
    loggers.setRoot();
}

Logger *Log::get(const char *name)
{
    return loggers.get(name);
}

void Log::log(const char *name, Log::Level level, const char *fmt, va_list args)
{
    get(name)->log(level, fmt, args);
}

void Log::log(const char *name, Log::Level level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    get(name)->log(level, fmt, ap);
    va_end(ap);
}

Logger::Logger(Loggers *loggers, Logger *parent, std::string &name) :
    m_loggers(loggers),
    m_parent(parent),
    m_name(name),
    m_level(Log::Unset)
{
}

Logger::Logger(Loggers *loggers, Logger *parent, const char *name, int32_t nameLen) :
    m_loggers(loggers),
    m_parent(parent),
    m_name(name, nameLen),
    m_level(Log::Unset)
{
}

void Logger::setLevel(Log::Level level)
{
    if (m_parent || level != Log::Unset)  // Can't unset root logger
        m_level = level;
}

Log::Level Logger::getLevel() const
{
    if (m_level != Log::Unset)
        return m_level;
    return m_parent->getLevel();
}

Logger *Logger::getParent()
{
    return m_parent;
}

void Logger::setAppender(LogAppender *a)
{
    assert(a);

    lock_guard<mutex> lock(m_loggers->m_lock);
    auto iter = m_appenders.find(a);
    if (iter != m_appenders.end())
        m_appenders.erase(iter);
    m_appenders.insert(a);
    a->setLoggers(m_loggers);
}

void Logger::clearAppender(LogAppender *logAppender)
{
    if (logAppender) {
        auto iter = m_appenders.find(logAppender);
        if (iter != m_appenders.end())
            m_appenders.erase(iter);
    } else {
        for (auto a : m_appenders) {
            a->setLoggers(0);
        }
        m_appenders.clear();
    }
}

void Logger::append(Log::Level level, std::string const &s)
{
    {
        lock_guard<mutex> lock(m_loggers->m_lock);
        for (auto a : m_appenders) {
            a->append(s);
        }
    }

    if (m_parent)
        m_parent->append(level, s);
}

static const char levelChar[] = {
    'T',
    'D',
    'I',
    'W',
    'E',
    'F'
};

void Logger::log(Log::Level level, const char *fmt, va_list ap)
{
    try {
        if (this && getLevel() <= level) {
#if 0
            std::string s;
            uint64_t usec64 = Clock::monotonicUSec();
            unsigned int usec = (unsigned int)(usec64 % 1000000);
            unsigned int sec = (unsigned int)(usec64 / 1000000);
            unsigned int min = sec / 60;
            sec = sec % 60;
            unsigned int hour = min / 60;
            min = min % 60;
            s.format("%02d:%02d:%02d.%06d %c %-10s ", hour, min, sec, usec, levelChar[level - 1],
                    m_name.c_str());
            s.appendFormatList(fmt, ap);
#else
            std::string s(format("%c %-10s ", levelChar[level - 1], m_name.c_str()));
            appendFormatList(s, fmt, ap);
#endif
            s += "\n";
            append(level, s);
        }
    } catch (...) {
        // Logging is best effort.
    }
}

void Logger::log(Log::Level level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log(level, fmt, ap);
    va_end(ap);
}

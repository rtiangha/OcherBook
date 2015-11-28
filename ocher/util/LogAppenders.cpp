#include <new>

#include "util/LogAppenderFile.h"
#include "util/Path.h"


LogAppenderFile::LogAppenderFile(const char *filename) :
    m_file(Path::join(Log::getLogDirectory().c_str(), filename), "w+")
{
}

LogAppenderFile::~LogAppenderFile()
{
    detach();
}

void LogAppenderFile::append(std::string const &s)
{
    m_file.write(s.c_str(), s.length());
    m_file.flush();
}

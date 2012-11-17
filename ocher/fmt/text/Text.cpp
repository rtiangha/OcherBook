#include "clc/storage/File.h"
#include "clc/support/Logger.h"
#include "ocher/fmt/text/Text.h"

#define LOG_NAME "ocher.fmt.text"


Text::Text(const char* filename)
{
    clc::File f(filename);
    f.readRest(m_text);
    clc::Log::debug(LOG_NAME, "Loaded %u bytes", m_text.size());
}

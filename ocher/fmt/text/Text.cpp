/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "fmt/text/Text.h"

#include "util/Logger.h"

#include <fstream>
#include <sstream>

#define LOG_NAME "ocher.text"


Text::Text(const std::string& filename)
{
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_text = buffer.str();
    Log::debug(LOG_NAME, "Loaded %zu bytes", m_text.size());
}

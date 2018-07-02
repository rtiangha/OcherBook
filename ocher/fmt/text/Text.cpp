/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "fmt/text/Text.h"

#include "util/File.h"
#include "util/Logger.h"

#define LOG_NAME "ocher.text"


Text::Text(const std::string& filename)
{
    File f(filename);

    f.readRest(m_text);
    Log::debug(LOG_NAME, "Loaded %zu bytes", m_text.size());
}

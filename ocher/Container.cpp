/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "Container.h"

Container::Container() :
    settings(filesystem),
    powerSaver(loop)
#ifdef OCHER_TARGET_KOBO
    , koboEvents(loop)
#endif
{
}

std::unique_ptr<Container> g_container;

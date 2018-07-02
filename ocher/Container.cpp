/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "Container.h"

Container::Container() :
    settings(filesystem),
    powerSaver(loop)
{
}

Container g_container;

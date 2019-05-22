/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ux/Renderer.h"

#include "fmt/Layout.h"

void Renderer::set(std::unique_ptr<Layout> layout)
{
    m_layout = std::move(layout);
}

#if 0
void Renderer::pushOp(uint16_t op)
{
}

void Renderer::pushTextAttr(TextAttr attr, uint8_t arg)
{
    pushOp((uint16_t)attr | (arg << 8));
}
#endif

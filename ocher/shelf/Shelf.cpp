#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/shelf/Meta.h"
#include "ocher/shelf/Shelf.h"

#define LOG_NAME "ocher.shelf"


Shelf::~Shelf()
{
    size_t n = m_meta.countItems();
    for (size_t i = 0; i < n; ++i) {
        delete (Meta*)m_meta.get(i);
    }
}

void Shelf::add(Meta* meta)
{
    m_meta.add(meta);
}

void Shelf::markActive(Meta* meta)
{
    size_t i = m_meta.indexOf(meta);
    ASSERT(i != clc::List::NotFound);
    m_meta.moveItem(i, 0);
}

void Shelf::select(Meta* meta)
{
    m_selected = meta;
    clc::Log::info(LOG_NAME, "Selected %s", meta->title.c_str());
}

Meta* Shelf::selected()
{
    return m_selected;
}

void Shelf::updateShortList()
{
}

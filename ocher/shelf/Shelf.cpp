#include "clc/support/Debug.h"
#include "clc/support/Logger.h"
#include "ocher/shelf/Meta.h"
#include "ocher/shelf/Shelf.h"

#define LOG_NAME "ocher.shelf"


Shelf::Shelf()
{
}

Shelf::~Shelf()
{
}

Library library;
Library* g_library = &library;


Library::~Library()
{
    size_t n = m_meta.countItems();
    for (size_t i = 0; i < n; ++i) {
        delete (Meta*)m_meta.get(i);
    }
}

void Library::add(Meta* meta)
{
    m_meta.add(meta);
}

void Library::select(Meta* meta)
{
    m_selected = meta;
    clc::Log::info(LOG_NAME, "Selected %s", meta->title.c_str());
}

Meta* Library::selected()
{
    return m_selected;
}


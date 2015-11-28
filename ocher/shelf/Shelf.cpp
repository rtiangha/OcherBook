/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "ocher/shelf/Meta.h"
#include "ocher/shelf/Shelf.h"
#include "ocher/util/Debug.h"
#include "ocher/util/Logger.h"

#define LOG_NAME "ocher.shelf"


void GroupOfBooks::attach(GroupOfBooks *observer)
{
    m_observers.push_back(observer);
}

void GroupOfBooks::detach(GroupOfBooks *observer)
{
    for (auto it = m_observers.begin(); it < m_observers.end(); ++it) {
        if ((*it) == observer) {
            m_observers.erase(it);
            break;
        }
    }
}

void GroupOfBooks::notify()
{
    unsigned int n = m_observers.size();

    for (unsigned int i = 0; i < n; ++i) {
        GroupOfBooks *group = m_observers[i];
        group->update(this);
    }
}


Shelf::Shelf()
{
}

Shelf::~Shelf()
{
}


Library::~Library()
{
    size_t n = m_meta.size();

    for (size_t i = 0; i < n; ++i) {
        delete m_meta[i];
    }

    notify();
}

void Library::add(Meta *meta)
{
    m_meta.push_back(meta);
}


void ShortList::update(GroupOfBooks *changed)
{
    m_meta.clear();
    const std::vector<Meta *> *books = changed->getList();
    unsigned int n = books->size();
    for (unsigned int i = 0; i < n; ++i) {
        Meta *meta = (*books)[i];
        if (meta->record.shortlist) {
            m_meta.push_back(meta);
        }
    }
}

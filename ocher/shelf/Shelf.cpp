/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#include "shelf/Shelf.h"

#include "shelf/Meta.h"
#include "util/Debug.h"
#include "util/Logger.h"

#define LOG_NAME "ocher.shelf"


void GroupOfBooks::attach(GroupOfBooks* observer)
{
    m_observers.push_back(observer);
}

void GroupOfBooks::detach(GroupOfBooks* observer)
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
    for (GroupOfBooks* group : m_observers)
        group->update(this);
}


Library::~Library()
{
    for (Meta* meta : m_meta)
        delete meta;

    notify();
}

void Library::add(Meta* meta)
{
    m_meta.push_back(meta);
}


void ShortList::update(GroupOfBooks* changed)
{
    m_meta.clear();
    const auto& books = changed->getList();
    for (auto meta : books) {
        if (meta->record.shortlist) {
            m_meta.push_back(meta);
        }
    }
}

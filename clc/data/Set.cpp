#include <new>

#include "clc/data/Set.h"

namespace clc
{

bool Set::contains(void* item) const
{
    char** items = (char**)m_set.items();
    unsigned int n = m_set.countItems();
    for (unsigned int i = 0; i < n; ++i) {
        if (items[i] == item)
            return true;
    }
    return false;
}

bool Set::add(void* item)
{
    if (!contains(item)) {
        if (m_set.add(item))
            return true;
        throw std::bad_alloc();
    }
    return false;
}

void* Set::remove()
{
    ASSERT(m_set.countItems());
    return m_set.removeItem(m_set.countItems()-1);
}

bool Set::removeAll(Set const& s)
{
    bool changed = false;
    char** items = (char**)s.m_set.items();
    unsigned int n = s.m_set.countItems();
    for (unsigned int i = 0; i < n; ++i) {
        changed |= m_set.removeItem((void*) items[i]);
    }
    return changed;
}

}

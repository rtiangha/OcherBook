#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clc/data/List.h"


namespace clc
{

static inline void moveItems(void** items, int offset, size_t count)
{
    if (count > 0 && offset != 0)
        memmove(items + offset, items, count * sizeof(void*));
}


List::List(size_t capacity) :
    m_list(0),
    m_allocated(0),
    m_used(0)
{
    _resizeArray(capacity);
}


List::List(const List& list) :
    m_list(0),
    m_allocated(0),
    m_used(0)
{
    *this = list;
}


List::~List()
{
    free(m_list);
}


List& List::operator=(const List &list)
{
    if (m_list) {
        free(m_list);
        m_list = 0;
    }
    _resizeArray(list.m_used);
    m_used = list.m_used;
    memcpy(m_list, list.m_list, m_used * sizeof(void*));
    return *this;
}


bool List::addAt(void *item, size_t index)
{
    bool result = true;
    if (index >= m_allocated)
        result = _resizeArray(index+1);
    if (result) {
        moveItems(m_list + index, 1, m_used - index - 1);
        m_list[index] = item;
        if (index >= m_used)
            m_used = index+1;
    }
    return result;
}


bool List::add(void *item)
{
    bool result = true;
    if (m_allocated > m_used) {
        m_list[m_used] = item;
        ++m_used;
    } else {
        if ((result = _resizeArray(m_used + 1))) {
            m_list[m_used] = item;
            ++m_used;
        }
    }
    return result;
}


bool List::addList(const List *list, size_t index)
{
    bool result = (list && index <= m_used);
    if (result && list->m_used > 0) {
        size_t count = list->m_used;
        if (m_used + count > m_allocated)
            result = _resizeArray(m_used + count);
        if (result) {
            m_used += count;
            moveItems(m_list + index, count, m_used - index - count);
            memcpy(m_list + index, list->m_list,
                   list->m_used * sizeof(void *));
        }
    }
    return result;
}


bool List::addList(const List *list)
{
    bool result = (list != 0);
    if (result && list->m_used > 0) {
        size_t index = m_used;
        size_t count = list->m_used;
        if (m_used + count > m_allocated)
            result = _resizeArray(m_used + count);
        if (result) {
            m_used += count;
            memcpy(m_list + index, list->m_list,
                   list->m_used * sizeof(void *));
        }
    }
    return result;
}


List* List::split(size_t index)
{
    List* l = new List;
    split(index, l);
    return l;
}


bool List::split(size_t index, List* tail)
{
    bool result = true;
    tail->m_used = 0;
    if (index < m_used) {
        size_t n = m_used - index;
        if (tail->m_allocated < n) {
            result = tail->_resizeArray(n);
        }
        if (result) {
            tail->m_used = n;
            memcpy(tail->m_list, m_list+n, n*sizeof(void*));
            m_used -= n;
        }
    }
    return result;
}


void* List::remove()
{
    if (m_used)
        return removeAt(m_used-1);
    return 0;
}

bool List::removeItem(void *item)
{
    size_t index = indexOf(item);
    if (index != NotFound)
        removeAt(index);
    return (index != NotFound);
}


void* List::removeAt(size_t index)
{
    void* item;
    if (index < m_used) {
        item = m_list[index];
        moveItems(m_list + index + 1, -1, m_used - index - 1);
        --m_used;
        if (m_used <= m_allocated>>2)
            _resizeArray(m_used);
    } else
        item = 0;
    return item;
}


bool List::removeItems(size_t index, size_t count)
{
    bool result = index <= m_used;
    if (result) {
        if (index + count > m_used)
            count = m_used - index;
        moveItems(m_list + index + count, -count,
                   m_used - index - count);
        m_used -= count;
        if (m_used <= m_allocated>>2)
            _resizeArray(m_used);
    }
    return result;
}


bool List::replaceItem(size_t index, void* newItem)
{
    bool result = false;

    if (index < m_allocated) {
        m_list[index] = newItem;
        result = true;
        if (index >= m_used)
            m_used = index+1;
    }
    return result;
}


void List::clear()
{
    m_used = 0;
    _resizeArray(0);
}


void List::sortItems(int (*compareFunc)(const void *, const void *))
{
    if (compareFunc)
        qsort(m_list, m_used, sizeof(void *), compareFunc);
}


bool List::swapItems(size_t indexA, size_t indexB)
{
    bool result = false;

    if (indexA < m_used && indexB < m_used) {
        void *tmpItem = m_list[indexA];
        m_list[indexA] = m_list[indexB];
        m_list[indexB] = tmpItem;

        result = true;
    }

    return result;
}


bool List::moveItem(size_t fromIndex, size_t toIndex)
{
    if ((fromIndex >= m_used) || (toIndex >= m_used))
        return false;

    if (fromIndex < toIndex)
    {
        void * tmp_mover = m_list[fromIndex];
        memmove(m_list + fromIndex + 1, m_list + fromIndex, (toIndex - fromIndex) * sizeof(void *));
        m_list[toIndex] = tmp_mover;
    }
    else if (fromIndex > toIndex)
    {
        void * tmp_mover = m_list[fromIndex];
        memmove(m_list + toIndex + 1, m_list + toIndex, (fromIndex - toIndex) * sizeof(void *));
        m_list[toIndex] = tmp_mover;
    };
    return true;
}


void* List::itemAt(int index) const
{
    void *item;
    if (index < 0)
        index = (int)m_used + index;
    if (index >= 0 && (size_t)index < m_used)
        item = m_list[index];
    else
        item = 0;
    return item;
}


void* List::firstItem() const
{
    void *item = 0;
    if (m_used > 0)
        item = m_list[0];
    return item;
}


void* List::lastItem() const
{
    void *item = 0;
    if (m_used > 0)
        item = m_list[m_used - 1];
    return item;
}


size_t List::indexOf(void *item) const
{
    for (size_t i = 0; i < m_used; i++) {
        if (m_list[i] == item)
            return i;
    }
    return NotFound;
}


void List::doForEach(bool (*func)(void* item))
{
    if (func != 0) {
        bool terminate = false;
        size_t index = 0;
        while ((!terminate) && (index < m_used)) {
            terminate = func(m_list[index]);
            index++;
        };
    }

}


void List::doForEach(bool (*func)(void* item, void* arg), void * arg)
{
    if (func != 0) {
        bool terminate = false;
        size_t index = 0;
        while ((!terminate) && (index < m_used)) {
            terminate = func(m_list[index], arg);
            index++;
        };
    }
}

void List::setSize(size_t count)
{
    _resizeArray(count);
    m_used = count;
}

bool List::_resizeArray(size_t count)
{
    bool result = true;
    size_t newSize = m_allocated;
    if (count > m_allocated) {
        newSize++;
        while (newSize < count)
            newSize <<= 1;
    } else if (count <= m_allocated>>2) {
        newSize = count;
    }
    // Never go down to 0, because realloc of 0 frees.
    if (newSize == 0)
        newSize = 1;

    // resize if necessary
    if (newSize != m_allocated) {
        void** newObjectList = (void**)realloc(m_list, newSize * sizeof(void*));
        if (newObjectList) {
            m_list = newObjectList;
            m_allocated = newSize;
            if (m_used > m_allocated)
                m_used = m_allocated;
        } else {
            result = false;
        }
    }
    return result;
}

}


#ifndef LIBCLC_LIST_H
#define LIBCLC_LIST_H

#include <stddef.h>

namespace clc
{

/**
 *  Light-weight list of pointers.  Grows automatically to accomodate new indicies.
 *  Does not auto-zero skipped indicies.
 */
class List
{
public:
    List(size_t capacity = 16);
    List(const List& anotherList);
    ~List();

    List& operator=(const List &);

    void clear();
    bool addAt(void* item, size_t index);
    bool add(void* item);
    bool addList(const List* list, size_t index);
    bool addList(const List* list);
    List* split(size_t index);
    bool split(size_t index, List* tail);
    bool replaceItem(size_t index, void* newItem);
    void* remove();
    bool removeItem(void* item);
    void* removeAt(size_t index);
    bool removeItems(size_t index, size_t count);
    void sortItems(int (*compareFunc)(const void*, const void*));
    bool swapItems(size_t indexA, size_t indexB);
    /**
     * This moves a list item from posititon a to position b, moving the appropriate block of
     *  list elements to make up for the move.  For example, in the array:
     *  A B C D E F G H I J
     *  Moving 1(B)->6(G) would result in this:
     * A C D E F G B H I J
     */
    bool moveItem(size_t fromIndex, size_t toIndex);
    /**
     * @param index  0..size-1, or -1..-size from end
     * @return Pointer at that index, or NULL if invalid index
     */
    void* itemAt(int index) const;
    void* get(size_t index) const { return itemAt(index); }
    void* firstItem() const;
    void* itemAtFast(size_t index) const { return m_list[index]; }
    void* lastItem() const;
    void* items() const { return m_list; }
    bool hasItem(void* item) const { return (indexOf(item) != NotFound); }
    size_t indexOf(void* item) const;
    size_t countItems() const { return m_used; }
    size_t size() const { return m_used; }
    size_t length() const { return m_used; }
    bool isEmpty() const { return (m_used == 0); }
    /**
     *  iterate a function over the whole list.  If the function outputs a true
     * value, then the process is terminated.
     */
    void doForEach(bool (*func)(void* item));
    void doForEach(bool (*func)(void* item, void* arg), void *arg);
    void setSize(size_t count);

private:
    bool _resizeArray(size_t count);

    void** m_list;
    size_t m_allocated;
    size_t m_used;

public:
    static const size_t NotFound = ((size_t)-1);
};


}

#endif

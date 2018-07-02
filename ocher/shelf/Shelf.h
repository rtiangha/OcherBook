/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_SHELF_H
#define OCHER_SHELF_H

#include <vector>


class Meta;

/** A grouping of books (physical or logical).
 *
 * Groups are both both subject and observer (groups can potentially aggregate and filter other
 * groups).
 */
class GroupOfBooks {
public:
    virtual ~GroupOfBooks() = default;

    void attach(GroupOfBooks* observer);
    void detach(GroupOfBooks* observer);
    void notify();

    // TODO: updateMetadata vs updateMembership ?
    virtual void update(GroupOfBooks*)
    {
    }

    virtual const std::vector<Meta*>& getList() const = 0;

protected:
    std::vector<GroupOfBooks*> m_observers;
};


/** A Shelf is a filtered, sorted, "view" of other GroupOfBooks.
 */
class Shelf : public GroupOfBooks {
public:
    Shelf() = default;
    ~Shelf() = default;

    enum class SortBy {
        Author,
        Title,
    };

    /**
     * Shelves watch other GroupOfBooks and must implement update().
     */
    virtual void update(GroupOfBooks* changed) override = 0;

#if 0
    addFilterTag();
    removeFilterTag();
    sort();
    clearSort();
#endif
};


/** The Library contains references to all of the user's books.
 *
 * Unlike a Shelf, it owns the actual books.
 */
class Library : public GroupOfBooks {
public:
    Library()
    {
    }

    ~Library();

    /** Adds the metadata to the Library.  Ownership is transferred.
     * Caller should call notify() when done adding.
     */
    void add(Meta*);

    const std::vector<Meta*>& getList() const override
    {
        return m_meta;
    }

protected:
    std::vector<Meta*> m_meta;
};


/** A ShortList is a Shelf of books the user intends to read.
 */
class ShortList : public Shelf {
public:
    ShortList(GroupOfBooks* base) :
        m_base(base)
    {
        m_base->attach(this);
    }

    ~ShortList()
    {
        m_base->detach(this);
    }

    void update(GroupOfBooks* changed) override;

    const std::vector<Meta*>& getList() const override
    {
        return m_meta;
    }

protected:
    std::vector<Meta*> m_meta;
    GroupOfBooks* m_base;
};

#endif

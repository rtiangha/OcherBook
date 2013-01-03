#ifndef OCHER_SHELF_H
#define OCHER_SHELF_H

#include "clc/data/List.h"

class Meta;

/**
 * A Shelf is a filtered, sorted, read-only "view" of other Shelves (or as a base case, the
 * global Library).
 */
class Shelf
{
public:
    Shelf();
    ~Shelf();

    // refresh: check, re-trigger filter

    // track underlying sources

    // set/remove tags, ...: re-trigger filter and observers

    // filter on tags

    // when changing, invalidate observers and trigger then to refilter
};

/**
 * The Library contains references to all of the user's books.  It is a specialization of Shelf in
 * that it has no backing sources; it owns the books.
 */
class Library : public Shelf
{
public:
    Library() : m_selected(0) {}
    ~Library();

    // TODO shelf

    /**
     * Adds the metadata to the Library.  Ownership is transferred.
     */
    void add(Meta*);

    void select(Meta*);
    Meta* selected();
    Meta* m_selected;

    clc::List m_meta;
};

#endif

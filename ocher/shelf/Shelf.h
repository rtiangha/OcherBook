#ifndef OCHER_SHELF_H
#define OCHER_SHELF_H

#include "clc/data/List.h"

class Meta;

class Shelf
{
public:
    Shelf() : m_selected(0) {}
    ~Shelf();

    /**
     */
    void add(Meta*);

    /**
     */
    void markActive(Meta*);

    void select(Meta*);
    Meta* selected();
    Meta* m_selected;

    clc::List m_meta;

    void updateShortList();
    clc::List m_shortList;
};

#endif

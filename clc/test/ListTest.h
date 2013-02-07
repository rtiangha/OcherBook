#include "clc/data/List.h"


SUITE(List)
{
    TEST(constructor)
    {
        PRINT_PROGRESS;
        clc::List l;
        CHECK(l.isEmpty());
        CHECK_EQUAL(0U, l.size());
    }

    TEST(addIndex)
    {
        PRINT_PROGRESS;
        clc::List l;
        l.add((void*)123);
        l.add((void*)789);
        l.addAt((void*)456, 1);

        CHECK_EQUAL((void*)123, l.get(0));
        CHECK_EQUAL((void*)456, l.get(1));
        CHECK_EQUAL((void*)789, l.get(2));

    }

    TEST(hasItem)
    {
        PRINT_PROGRESS;
        clc::List l;
        CHECK(! l.hasItem((void*)0));
        l.add((void*)0);
        CHECK(l.hasItem((void*)0));
        CHECK(! l.hasItem((void*)123));
        l.add((void*)123);
        CHECK(l.hasItem((void*)123));
    }

    TEST(clear)
    {
        PRINT_PROGRESS;
        clc::List l;
        CHECK(l.isEmpty());
        l.add((void*)123);
        CHECK(! l.isEmpty());
        l.clear();
        CHECK(l.isEmpty());
    }

    TEST(mixed)
    {
        PRINT_PROGRESS;
        clc::List l;

        l.add((void*)123);
        CHECK(! l.isEmpty());
        CHECK_EQUAL(1U, l.size());
        CHECK_EQUAL((void*)123, l.get(0));

        l.add((void*)456);
        CHECK_EQUAL(2U, l.size());
        CHECK_EQUAL((void*)123, l.get(0));
        CHECK_EQUAL((void*)456, l.get(1));

        l.replaceItem(1, (void*)789);
        CHECK_EQUAL((void*)123, l.get(0));
        CHECK_EQUAL((void*)789, l.get(1));

        l.removeAt(0U);
        CHECK_EQUAL(1U, l.size());
        CHECK_EQUAL((void*)789, l.get(0));

        l.clear();
        CHECK(l.isEmpty());
    }

    // TODO:
    // void* remove()
    // void* remove(unsigned int index)
    // bool remove(void* o)
    // void set(unsigned int index, void* o)
}

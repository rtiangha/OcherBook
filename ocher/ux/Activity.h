/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_ACTIVITY_H
#define OCHER_UX_ACTIVITY_H


/** A logical portion of user interaction.
 */
class Activity {
public:
    enum class Type {
        Boot,
        Sleep,
        Sync,
        Home,
        Read,
        Library,
        Settings,
        Previous,
        Quit,
    };

    virtual ~Activity() = default;

    virtual void onAttached() = 0;
    virtual void onDetached() = 0;
};

#endif

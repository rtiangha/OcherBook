/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CDK_LIBRARYACTIVITY_H
#define OCHER_UX_CDK_LIBRARYACTIVITY_H

#include "ux/cdk/ActivityCdk.h"

#include <vector>

class Meta;

class LibraryActivityCdk : public ActivityCdk {
public:
    LibraryActivityCdk(UxControllerCdk* uxController);

    ~LibraryActivityCdk()
    {
    }

    void onAttached();
    void onDetached();

    Meta* browse(std::vector<Meta*>& meta);
    void read(Meta* meta);
};

#endif

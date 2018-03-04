/*
 * Copyright (c) 2016, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CDK_HOMEACTIVITY_H
#define OCHER_UX_CDK_HOMEACTIVITY_H

#include "ux/cdk/ActivityCdk.h"


class HomeActivityCdk : public ActivityCdk {
public:
    HomeActivityCdk(UxControllerCdk* c);
    ~HomeActivityCdk();
};

#endif


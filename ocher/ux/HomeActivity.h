#ifndef OCHER_UX_HOMEACTIVITY_H
#define OCHER_UX_HOMEACTIVITY_H

#include "ocher/ux/Activity.h"


class HomeActivity
{
public:
    HomeActivity(Controller* c);

    Activity run();

protected:
    Controller* m_controller;
};

#endif


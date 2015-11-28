/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CDK_UXCONTROLLER_H
#define OCHER_UX_CDK_UXCONTROLLER_H

#include "ocher/ux/Controller.h"

#include <cdk.h>

/**
 */
class UxControllerCdk : public UxController {
public:
    UxControllerCdk();
    ~UxControllerCdk();

    const char *getName() const
    {
        return "cdk";
    }

    bool init();

    Renderer *getRenderer()
    {
        return m_renderer;
    }

    void run(enum ActivityType a);

protected:
    std::string m_name;
    WINDOW *m_scr;
    CDKSCREEN *m_screen;

    Renderer *m_renderer;
};

#endif

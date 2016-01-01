/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FD_UXCONTROLLER_H
#define OCHER_UX_FD_UXCONTROLLER_H

#include "ocher/ux/Controller.h"

class Renderer;

/**
 */
class UxControllerFd : public UxController {
public:
    UxControllerFd();
    ~UxControllerFd();

    const char *getName() const
    {
        return "fd";
    }

    bool init();

    Renderer *getRenderer()
    {
        return m_renderer;
    }

    void setNextActivity(enum ActivityType a);

protected:
    Renderer *m_renderer;
};

#endif

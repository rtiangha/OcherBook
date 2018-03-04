/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_CDK_UXCONTROLLER_H
#define OCHER_UX_CDK_UXCONTROLLER_H

#include "ux/Controller.h"

#include <cdk.h>

#include <string>

class RendererCdk;

/** Controller for the CDK user experience.  Manages interactions with CDK on a dedicated thread.
 */
class UxControllerCdk : public UxController {
public:
    UxControllerCdk();
    ~UxControllerCdk();

    const char* getName() const
    {
        return "cdk";
    }

    bool init();

    Renderer* getRenderer()
    {
        return m_renderer;
    }

    void setNextActivity(enum ActivityType a);

    WINDOW* m_scr;
    CDKSCREEN* m_screen;

protected:
    std::string m_name;

    Renderer* m_renderer;
};

#endif

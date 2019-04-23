/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FD_UXCONTROLLER_H
#define OCHER_UX_FD_UXCONTROLLER_H

#include "ux/Controller.h"
#include "ux/Renderer.h"

#include <string>

/**
 */
class UxControllerFd : public UxController {
public:
    UxControllerFd() = default;
    ~UxControllerFd() = default;

    const std::string& getName() const override
    {
        return m_name;
    }

    bool init() override;

    Renderer* getRenderer() override
    {
        return m_renderer.get();
    }

    Activity::Type previousActivity() override { return m_activityType; }
    void setNextActivity(Activity::Type a) override;

protected:
    std::string m_name = "fd";
    std::unique_ptr<Renderer> m_renderer;
    Activity::Type m_activityType = Activity::Type::Quit;
};

#endif

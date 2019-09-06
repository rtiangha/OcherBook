/*
 * Copyright (c) 2015, Chuck Coffing
 * OcherBook is released under the GPLv3.  See COPYING.
 */

#ifndef OCHER_UX_FB_SYSTEMBAR_H
#define OCHER_UX_FB_SYSTEMBAR_H

#include "ux/fb/Widgets.h"

#include <string>

class Battery;
class UxControllerFb;

class SystemBar : public Window {
public:
    SystemBar(UxControllerFb* uxController, Battery& battery);

    bool m_sep;

protected:
    void drawContent(const Rect*) override;

    UxControllerFb* m_uxController;
};

#endif

#include "ocher/ux/SettingsActivity.h"

SettingsActivity::SettingsActivity(Controller* c) :
    m_controller(c)
{
}

Activity SettingsActivity::run()
{
    return ACTIVITY_HOME;
}

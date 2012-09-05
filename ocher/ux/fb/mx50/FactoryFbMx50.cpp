#include "ocher/ocher.h"
#include "ocher/ux/fb/FactoryFbMx50.h"

UX_DRIVER_REGISTER(FbMx50);


UiFactoryFbMx50::UiFactoryFbMx50() :
    UiFactoryFb(),
    m_fb(0)
{
}

UiFactoryFbMx50::~UiFactoryFbMx50()
{
}

bool UiFactoryFbMx50::init()
{
    m_fb = new Mx50Fb;
    if (UiFactoryFb::init())
        return true;
    delete m_fb;
    return false;
}

const char* UiFactoryFbMx50::getName()
{
    return "kobo-mx50";
}


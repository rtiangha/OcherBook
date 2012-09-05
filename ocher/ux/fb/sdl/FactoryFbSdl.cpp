#include "ocher/ocher.h"
#include "ocher/ux/fb/sdl/FactoryFbSdl.h"


UX_DRIVER_REGISTER(FbSdl);


UiFactoryFbSdl::UiFactoryFbSdl() :
    UiFactoryFb(),
    m_fb(0)
{
}

UiFactoryFbSdl::~UiFactoryFbSdl()
{
}

bool UiFactoryFbSdl::init()
{
    m_fb = new FbSdl();
    if (UiFactoryFb::init())
        return true;
    delete m_fb;
    return false;
}

const char* UiFactoryFbSdl::getName()
{
    return "sdl";
}



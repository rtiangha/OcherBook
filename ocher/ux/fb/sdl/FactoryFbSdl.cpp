#include "ocher/ocher.h"
#include "ocher/ux/fb/sdl/FactoryFbSdl.h"
#include "ocher/ux/fb/sdl/SdlLoop.h"


UX_DRIVER_REGISTER(FbSdl);


UiFactoryFbSdl::UiFactoryFbSdl() :
    UiFactoryFb()
{
}

UiFactoryFbSdl::~UiFactoryFbSdl()
{
}

bool UiFactoryFbSdl::init()
{
    FbSdl* fbSdl = new FbSdl();
    if (fbSdl->init()) {
        m_fb = fbSdl;
        if (UiFactoryFb::init()) {
            m_loop = new SdlLoop;
            return true;
        }
    }
    delete fbSdl;
    return false;
}

const char* UiFactoryFbSdl::getName()
{
    return "sdl";
}



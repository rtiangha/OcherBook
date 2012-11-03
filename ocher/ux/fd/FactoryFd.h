#ifndef OCHER_UX_FACTORY_FD_H
#define OCHER_UX_FACTORY_FD_H

#include "ocher/ux/Factory.h"
#include "ocher/ux/fd/RenderFd.h"


class UiFactoryFd : public UiFactory
{
public:
    UiFactoryFd();
    virtual ~UiFactoryFd() {}

    bool init();
    void deinit();
    const char* getName();
    Renderer* getRenderer();

protected:
    RendererFd* m_renderer;
};

#endif



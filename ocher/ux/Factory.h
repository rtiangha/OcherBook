#ifndef OCHER_UX_FACTORY_H
#define OCHER_UX_FACTORY_H

#include "ocher/ux/Renderer.h"
#include "ocher/ux/Controller.h"

class EventLoop;
class FreeType;
class FrameBuffer;

// for ease of access
extern FrameBuffer* g_fb;
extern FreeType* g_ft;
extern EventLoop* g_loop;
extern Controller* g_cont;

class UiFactory
{
public:
    virtual ~UiFactory() {}

    /**
     * Called after user options have been parsed.
     * @return True iff sucessfully initialized, else output msg
     */
    virtual bool init() = 0;

    virtual void deinit() = 0;

    virtual const char* getName() = 0;

    virtual Renderer* getRenderer() = 0;

    virtual EventLoop* getLoop() { return (EventLoop*)0; }

    virtual FrameBuffer* getFrameBuffer() { return (FrameBuffer*)0; }

    virtual FreeType* getFontEngine() { return (FreeType*)0; }
};

extern UiFactory* uiFactory;

#endif


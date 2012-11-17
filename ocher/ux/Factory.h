#ifndef OCHER_UX_FACTORY_H
#define OCHER_UX_FACTORY_H

#include "ocher/shelf/Shelf.h"
#include "ocher/ux/Renderer.h"
#include "ocher/ux/fb/FrameBuffer.h"
#include "ocher/ux/fb/FreeType.h"


class EventLoop;

extern EventLoop* g_loop;
extern FrameBuffer* g_fb;
extern FreeType* g_ft;
extern Shelf* g_shelf;

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

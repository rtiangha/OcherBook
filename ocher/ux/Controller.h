#ifndef OCHER_CONTROLLER_H
#define OCHER_CONTROLLER_H

#include "ocher/ux/Factory.h"

class Controller
{
public:
    Controller(UiFactory *factory);

    void processFile(const char* file);
    void processFiles(const char** files);

    void run();

protected:
    void open();

    UiFactory *m_factory;

    clc::List m_meta;
};

#endif


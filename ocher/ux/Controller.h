#ifndef OCHER_CONTROLLER_H
#define OCHER_CONTROLLER_H


class Controller
{
public:
    Controller();

    void processFile(const char* file);
    void processFiles(const char** files);

    void run();

protected:
    void open();

    clc::List m_meta;
};

#endif


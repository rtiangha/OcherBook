#ifndef OCHER_FILESYSTEM_H
#define OCHER_FILESYSTEM_H


class Filesystem
{
public:
    Filesystem();
    ~Filesystem();

    const char **m_libraries;
    char* m_home;
    char* m_settings;
};

extern struct Filesystem fs;

#endif

#ifndef LIBCLC_OS_THREADLOCAL_H
#define LIBCLC_OS_THREADLOCAL_H

#include <pthread.h>

namespace clc
{

/**
 *  Portability wrapper for Thread Local Storage.
 */
class ThreadLocal
{
public:
    ThreadLocal(void (*destructor)(void*) = 0) : m_destructor(destructor)
    {
#ifdef _WIN32
        if ((m_key = TlsAlloc()) == TLS_OUT_OF_INDEXES)
            throw std::bad_alloc();
#else
        if (pthread_key_create(&m_key, m_destructor))
            throw std::bad_alloc();
#endif
    }

    ~ThreadLocal()
    {
#ifdef _WIN32
        TlsFree(m_key);
#else
        pthread_key_delete(m_key);
#endif
    }

    /**
     *  @return the thread local storage, or 0 if set() has not been called.
     */
    void* get()
    {
#ifdef _WIN32
        return TlsGetValue(m_key);
#else
        return pthread_getspecific(m_key);
#endif
    }

    void set(void* value)
    {
#ifdef _WIN32
        TlsSetValue(m_key, value);
#else
        pthread_setspecific(m_key, value);
#endif
    }

    /**
     *  Removes and destroys the data associated with this ThreadLocal.  This must be called before
     *  the thread exits, or the destructor may never be called, causing a memory leak.
     *  @note  If a value is set() but never remove()'d, it is unspecified whether or not the
     *      destructor callback is ever called.
     */
    void remove()
    {
        void* v = get();
        if (v)
        {
            if (m_destructor)
                m_destructor(v);
            set(0);
        }
    }

protected:
    void (*m_destructor)(void*);  ///< Callback that occurs to destroy set() data
#ifdef _WIN32
    DWORD m_key;
#else
    pthread_key_t m_key;
#endif
};

}

#endif


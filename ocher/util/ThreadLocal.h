#ifndef LIBCLC_OS_THREADLOCAL_H
#define LIBCLC_OS_THREADLOCAL_H

#include <new>
#include <pthread.h>

/**
 *  Portability wrapper for Thread Local Storage.
 */
class ThreadLocal {
public:
    ThreadLocal(void(*destructor)(void *) = 0) :
        m_destructor(destructor)
    {
        if (pthread_key_create(&m_key, m_destructor))
            throw std::bad_alloc();
    }

    ~ThreadLocal()
    {
        pthread_key_delete(m_key);
    }

    /**
     *  @return the thread local storage, or 0 if set() has not been called.
     */
    void *get()
    {
        return pthread_getspecific(m_key);
    }

    void set(void *value)
    {
        pthread_setspecific(m_key, value);
    }

    /**
     *  Removes and destroys the data associated with this ThreadLocal.  This must be called before
     *  the thread exits, or the destructor may never be called, causing a memory leak.
     *  @note  If a value is set() but never remove()'d, it is unspecified whether or not the
     *      destructor callback is ever called.
     */
    void remove()
    {
        void *v = get();

        if (v) {
            if (m_destructor)
                m_destructor(v);
            set(0);
        }
    }

protected:
    void (*m_destructor)(void *);  ///< Callback that occurs to destroy set() data
    pthread_key_t m_key;
};

#endif

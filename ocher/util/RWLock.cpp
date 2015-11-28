#include "util/RWLock.h"

#include <assert.h>
#include <new>
#include <string.h>


RWLock::RWLock()
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    memset(&m_rwlock, 0, sizeof(m_rwlock));
#else
    int r;
    if ((r = pthread_rwlock_init(&m_rwlock, NULL)))
        throw std::bad_alloc();
#endif
}


RWLock::~RWLock()
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    assert(canwlock(&m_rwlock));
#else
    int r = pthread_rwlock_destroy(&m_rwlock);
    assert(r == 0);
    (void)r;
#endif
}


void RWLock::readLock()
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    rlock(&m_rwlock);
    m_writer = false;
#else
    int r = pthread_rwlock_rdlock(&m_rwlock);
    assert(r == 0);
    (void)r;
#endif
}


void RWLock::writeLock()
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    wlock(&m_rwlock);
    m_writer = true;
#else
    int r = pthread_rwlock_rdlock(&m_rwlock);
    assert(r == 0);
    (void)r;
#endif
}


void RWLock::unlock()
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    if (m_writer)
        wunlock(&m_rwlock);
    else
        runlock(&m_rwlock);
#else
    int r = pthread_rwlock_unlock(&m_rwlock);
    assert(r == 0);
    (void)r;
#endif
}

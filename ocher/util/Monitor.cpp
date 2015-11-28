#include "util/Monitor.h"

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <time.h>


Monitor::Monitor() :
    m_locked(false),
    m_waiting(false)
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    memset(&m_r, 0, sizeof(m_r));
#else
//    m_lock = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
    int r;
    r = pthread_mutex_init(&m_lock, NULL);
    assert(r == 0);
    (void)r;
    r = pthread_cond_init(&m_cond, NULL);
    assert(r == 0);
    (void)r;
#endif
}


Monitor::~Monitor()
{
    assert(!m_locked);
    assert(!m_waiting);
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    // nothing to do
#else
    int r;
    r = pthread_cond_destroy(&m_cond);
    assert(r == 0);
    (void)r;
    r = pthread_mutex_destroy(&m_lock);
    assert(r == 0);
    (void)r;
#endif
}


void Monitor::notify()
{
    assert(m_locked);
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    taskwakeup(&m_r);
#else
    int r;
    r = pthread_cond_signal(&m_cond);
    assert(r == 0);
    (void)r;
#endif
}


void Monitor::notifyAll()
{
    assert(m_locked);
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    taskwakeupall(&m_r);
#else
    int r;
    r = pthread_cond_broadcast(&m_cond);
    assert(r == 0);
    (void)r;
#endif
}


void Monitor::wait()
{
    assert(m_locked);
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    m_waiting = true;
    m_locked = false;
    tasksleep(&m_r);
    m_locked = true;
    m_waiting = false;
#else
    m_locked = false;
    m_waiting = true;
    int r = pthread_cond_wait(&m_cond, &m_lock);
    assert(r == 0);
    (void)r;
    m_locked = true;
    m_waiting = false;
#endif
}

bool Monitor::tryLock()
{
    bool lockAcquired = false;

#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    // TODO
#else
    int r = pthread_mutex_trylock(&m_lock);
    if (r == 0) {
        m_locked = true;
        lockAcquired = true;
    } else {
        assert(r == EBUSY);
    }
#endif
    return lockAcquired;
}

void Monitor::lock()
{
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    // nothing to do
#else
    int r;
    r = pthread_mutex_lock(&m_lock);
    assert(r == 0);
    (void)r;
#endif
    m_locked = true;
}

void Monitor::unlock()
{
    assert(m_locked);
    m_locked = false;
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    // nothing to do
#else
    int r;
    r = pthread_mutex_unlock(&m_lock);
    assert(r == 0);
    (void)r;
#endif
}

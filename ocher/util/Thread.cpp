#include "util/Logger.h"
#include "util/StrUtil.h"
#include "util/Thread.h"

#ifdef USE_LIBTASK
#include <task.h>
#elif defined(__BEOS__) || defined(__HAIKU__)
#include <kernel/OS.h>
#else
#include <pthread.h>
#include <sched.h>
#endif
#include <assert.h>
#include <errno.h>
#include <new>
#include <stdio.h>
#include <unistd.h>


#if defined(__BEOS__) || defined(__HAIKU__)
inline int thread_join(thread_id &id, status_t &status)
{
    return wait_for_thread(id, &status);
}
inline void thread_usleep(unsigned int usec)
{
    snooze(usec);
}
#elif defined(USE_LIBTASK)
// TODO:  join should spin on id
typedef int *status_t;
inline int thread_join(thread_id &id, status_t &status)
{
    *status = 0;
    return 0;
}
inline void thread_usleep(unsigned int usec)
{
    taskdelay(usec / 1000);
}
#else
typedef void *status_t;
inline int thread_join(thread_id &id, status_t &status)
{
    return pthread_join(id, (void **)&status);
}
inline void thread_usleep(unsigned int usec)
{
    usleep(usec);
}
#endif


Thread::Thread()
{
    init();
    m_name.assign("");
    Debugger::nameThread(m_name.c_str());
}


Thread::Thread(char const *fmt, ...)
{
    init();
    va_list ap;
    va_start(ap, fmt);
    m_name = format(fmt, ap);
    va_end(ap);
    Debugger::nameThread(m_name.c_str());
}


Thread::~Thread()
{
    Log::debug("util.thread", "Thread \"%s\" destructor", m_name.c_str());

    if (m_state != JOINED) {
        interrupt();
        join();
    }
}


const std::string Thread::getName() const
{
    return m_name;
}

void Thread::interrupt()
{
    Log::debug("util.thread", "Thread \"%s\" interrupt", m_name.c_str());

    assert(m_state != JOINED);
    m_flags |= INTERRUPTED;
}


void Thread::join()
{
    Log::debug("util.thread", "Thread \"%s\" join", m_name.c_str());

    assert(currentThreadId() != m_thread);  // Can't join yourself
    m_stateChange.lock();
    if (m_state != JOINED) {
        while (m_state == STARTING) {
            m_stateChange.wait();
        }
        while (m_state == RUNNING) {
            m_stateChange.wait();
        }
        assert(m_state == STOPPED);
        status_t threadReturn;
        thread_join(m_thread, threadReturn);
        m_flags &= ~INTERRUPTED;
        m_state = JOINED;
        Log::debug("util.thread", "Thread \"%s\" is joined", m_name.c_str());
        m_thread = 0;
    }
    m_stateChange.unlock();
}


void Thread::setName(char const *fmt, ...)
{
    assert(m_state == JOINED);
    va_list ap;
    va_start(ap, fmt);
    m_name = format(fmt, ap);
    va_end(ap);
    Debugger::nameThread(m_name.c_str());
}


void Thread::sleep(unsigned int ms)
{
    assert(currentThreadId() == m_thread);  // Can only make yourself sleep
    thread_usleep(ms * 1000);
}


void Thread::_sleepUSec(unsigned int usec)
{
    thread_usleep(usec);
}


thread_id Thread::getThreadId()
{
    return m_thread;
}


void Thread::start()
{
    assert(m_state == JOINED);
    m_state = STARTING;
    Log::debug("util.thread", "Thread \"%s\" is starting", m_name.c_str());

    bool ok;
#ifdef USE_LIBTASK
    taskcreate(bootstrap, this, 32768);
    ok = true;
#elif defined(__BEOS__) || defined(__HAIKU__)
    m_stateChange.lock();
    m_thread = spawn_thread(bootstrap, m_name.c_str(), B_NORMAL_PRIORITY, this);
    ok = (m_thread != B_NO_MORE_THREADS && m_thread != B_NO_MEMORY);
    if (ok)
        resume_thread(m_thread);
    m_stateChange.unlock();
#else
    m_stateChange.lock();
    ok = (pthread_create(&m_thread, NULL, bootstrap, this) == 0);
    m_stateChange.unlock();
#endif
    if (!ok) {
        Log::debug("util.thread", "Thread \"%s\" failed to start; is joined", m_name.c_str());
        m_state = JOINED;
        m_thread = 0;
        throw std::bad_alloc();
    }
}


thread_status_t Thread::bootstrap(void *_self)
{
    Thread *self = static_cast<Thread *>(_self);
    Log::debug("util.thread", "Thread \"%s\" bootstrap", self->m_name.c_str());

    self->m_stateChange.lock();
#ifdef USE_LIBTASK
    taskname((char *)"%s", self->m_name.c_str());
#endif
    Log::debug("util.thread", "Thread \"%s\" is running", self->m_name.c_str());
    self->m_state = RUNNING;
    self->m_stateChange.notify();
    self->m_stateChange.unlock();
    try {
        self->run();
    } catch (...) {
        Log::warn("util.thread", "Thread \"%s\" threw exception", self->m_name.c_str());
    }
    Log::debug("util.thread", "Thread \"%s\" is stopped", self->m_name.c_str());
    self->m_stateChange.lock();
    self->m_state = STOPPED;
    self->m_stateChange.notify();
    self->m_stateChange.unlock();
#ifndef USE_LIBTASK
    return 0;
#endif
}


void Thread::init()
{
#ifdef USE_LIBTASK
    // nothing
#elif defined(__BEOS__) || defined(__HAIKU__)
    // nothing
#else
    // nothing
#endif
    m_state = JOINED;
    m_thread = 0;
    m_flags = 0;
}

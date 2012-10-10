#ifndef LIBCLC_RWLOCK_H
#define LIBCLC_RWLOCK_H

#if defined(__BEOS__) || defined(__HAIKU__)
#include <be/support/Locker.h>
#elif defined(USE_LIBTASK)
#include <task.h>
#else
#include <pthread.h>
#endif


namespace clc
{

/**
 *  A read/write lock.
 *  @note  Not exception safe; must always be explicitly unlocked.
 */
class RWLock
{
public:
    /**
     *  Constructor.  Lock starts out unlocked.
     */
    RWLock();

    /**
     *  Destructor.  Behavior is undefined if the lock is still locked.
     */
    ~RWLock();

    /**
     *  Lock for reader.  Multiple readers may have the lock simultaneously.
     *  Guaranteed that readers and writer may not simultaneously hold the lock.
     */
    void readLock();

    /**
     *  Lock for writer.  When the writer gets the lock, no other readers or writers exist.
     */
    void writeLock();

    /**
     *  Unlocks the read or write lock previously obtained by this thread.
     *  Behavior undefined if thread did not previously call readLock or writeLock.
     */
    void unlock();

protected:
#if defined(__BEOS__) || defined(__HAIKU__)
    // TODO
#elif defined(USE_LIBTASK)
    ::RWLock m_rwlock;
    bool m_writer;
#else
    pthread_rwlock_t m_rwlock;
#endif
};

}

#endif

/*
 * =====================================================================================
 *
 *       Filename:  Mutex.cpp
 *
 *    Description:  pthread mutex封装
 *
 *        Version:  1.0
 *        Created:  04/25/2011 09:39:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */


#include <cstdlib>
#include <cerrno>

#include "common/sys/Mutex.h"
#include "common/log/log.h"

Mutex::Mutex()
{
    pthread_mutexattr_t attr;

    int rt = pthread_mutexattr_init( &attr );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_mutexattr_init." );
    }

    rt = pthread_mutexattr_settype(
             &attr, PTHREAD_MUTEX_ERRORCHECK );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_mutexattr_settype." );
    }

    rt = pthread_mutex_init( &m_mutex, &attr );
    if ( rt != 0 )
    {
        DEBUG_LOG( "Syscall Error: pthread_mutex_init." );
    }

    rt = pthread_mutexattr_destroy( &attr );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_mutexattr_destroy." );
    }
}

Mutex::~Mutex()
{
    try
    {
        const int rt = pthread_mutex_destroy( &m_mutex );
        if ( rt != 0 )
        {
            DEBUG_LOG( "Error occurred whilst destroying a mutex." );
        }
    }
    catch ( ... )
    {
    }
}

void Mutex::lock() const
{
    const int rt = pthread_mutex_lock( &m_mutex );
    if ( rt != 0 )
    {
        if ( rt == EDEADLK )
        {
            DEBUG_LOG(
                "Try to acquire the mutex when the current thread already owns the mutex. " );
        }
        else
        {
            DEBUG_LOG( "Syscall Error: pthread_mutex_lock." );
        }
    }
}

bool Mutex::tryLock() const
{
    const int rt = pthread_mutex_trylock( &m_mutex );
    if ( rt != 0 && rt != EBUSY )
    {
        if ( rt == EDEADLK )
        {
            DEBUG_LOG(
                "Try to acquire the mutex when the current thread already owns the mutex. " );
        }
        else
        {
            DEBUG_LOG(
                "Syscall Error: pthread_mutex_trylock. Program will exit." );
        }
        return false;
    }

    return ( rt == 0 );
}

void Mutex::unlock() const
{
    const int rt = pthread_mutex_unlock( &m_mutex );
    if ( rt != 0 )
    {
        DEBUG_LOG( "Error occurred whilst unlock a mutex." );
    }
}

pthread_mutex_t* Mutex::getPthreadMutex() const
{
    return &m_mutex;
}


/*
 * =====================================================================================
 *
 *       Filename:  Cond.cpp
 *
 *    Description:  条件变量封装
 *
 *        Version:  1.0
 *        Created:  04/26/2011 10:20:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#include "common/sys/Mutex.h"
#include "common/sys/Time.h"
#include "common/sys/Cond.h"
#include "common/log/log.h"

Cond::Cond( const Mutex& mutex ) :
    m_mutex( mutex )
{
    pthread_condattr_t attr;
    int rt = pthread_condattr_init( &attr );

    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_condattr_init." );
    }

    rt =  pthread_cond_init( &m_cond, &attr );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_cond_init." );
    }

    rt = pthread_condattr_destroy( &attr );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_condattr_destroy." );
    }
}

Cond::~Cond()
{
    try
    {
        int rt = pthread_cond_destroy( &m_cond );
        if ( rt != 0)
        {
            ERROR_LOG("Cond::~Cond pthread_cond_destroy error");
        }
    }
    catch ( ... )
    {
    }
}

void Cond::notify() const
{
    const int rt = pthread_cond_signal( &m_cond );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_cond_signal." );
    }
}

void Cond::notifyAll() const
{
    const int rt = pthread_cond_broadcast( &m_cond );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_cond_broadcast." );
    }
}

bool Cond::wait() const
{
    int rt = pthread_cond_wait( &m_cond, m_mutex.getPthreadMutex() );

    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_cond_wait." );
        return false;
    }

    return true;
}

bool Cond::timedWait( const Time& timeout ) const
{
    if ( timeout < Time::microSeconds( 0 ) )
    {
        DEBUG_LOG( "Cond::timedWait: Invalid timeout argument." );
        return false;
    }

    struct timeval tv = Time::now( Time::Realtime ) + timeout;
    struct timespec ts;
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;

    const int rc = pthread_cond_timedwait(
                       &m_cond,
                       m_mutex.getPthreadMutex(),
                       &ts );

    if ( rc != 0 )
    {
        if ( rc != ETIMEDOUT )
        {
            DEBUG_LOG( "Syscall Error: pthread_cond_timedwait." );
            return false;
        }
    }
    return true;
}


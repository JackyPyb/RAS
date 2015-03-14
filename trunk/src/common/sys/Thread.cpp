/*
 * =====================================================================================
 *
 *       Filename:  Thread.cpp
 *
 *    Description:  linux线程简单封装
 *
 *        Version:  1.0
 *        Created:  04/26/2011 05:36:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */


#include <climits>
#include <sched.h>
#include <time.h>

#include "common/sys/Thread.h"
#include "common/sys/Time.h"
#include "common/log/log.h"
#include "common/comm/Error.h"

Thread::Thread() :
    m_bRunning( false ),
    m_bDetached( false )
{

}

Thread::~Thread()
{
    try
    {
        pthread_cancel( m_threadID.getRawID() );
        void* vp;
        join(&vp);
    }
    catch ( std::exception )
    {
        //throw;
    }
}

void* Thread::startHook( void* arg )
{
    Thread* pThread = static_cast<Thread*>( arg );
    pThread->run();
    pThread->done();
    return NULL;
}

int Thread::start( int stackSize )
{
    if ( m_bRunning )
    {
        DEBUG_LOG( "Try to start an already started thread." );
        return FAILED;
    }

    pthread_t tid;

    if ( stackSize > 0 )
    {
        pthread_attr_t attr;
        int rt = pthread_attr_init( &attr );

        if ( 0 != rt )
        {
            DEBUG_LOG( "Syscall Error: pthread_attr_init." );
            return FAILED;
        }

        if ( stackSize < PTHREAD_STACK_MIN )
        {
            stackSize = PTHREAD_STACK_MIN;
        }

        rt = pthread_attr_setstacksize( &attr, stackSize );

        if ( 0 != rt )
        {
            DEBUG_LOG( "Syscall Error: pthread_attr_setstatcksize." );
            return FAILED;
        }

        rt = pthread_create( &tid, &attr, startHook, this );

        if ( 0 != rt )
        {
            DEBUG_LOG( "Syscall Error: pthread_create." );
            return FAILED;
        }
    }
    else
    {
        const int rt = pthread_create( &tid, NULL, startHook, this );

        if ( 0 != rt )
        {
            DEBUG_LOG( "Syscall Error: pthread_create." );
            return FAILED;
        }
    }

    m_threadID = ThreadID( tid );

    m_bRunning = true;

    return SUCCESSFUL;

    // release lock
}

bool Thread::isAlive() const
{
    return m_bRunning;
}

void Thread::done()
{
    m_bRunning = false;
}

int Thread::join( void** valuePtr ) const
{
    if ( m_bDetached )
    {
        DEBUG_LOG( "Try to join a detached thread." );
        return FAILED;
    }

    const int rt = pthread_join( m_threadID.getRawID(), valuePtr );

    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_join." );
        return FAILED;
    }

    return SUCCESSFUL;
}

int Thread::detach() const
{
    if ( m_bDetached )
    {
        DEBUG_LOG( "Try to detach an already detached thread." );
        return FAILED;
    }

    const int rt = pthread_detach( m_threadID.getRawID() );
    //pthread_exit(0);
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pthread_detach." );
        return FAILED;
    }
    
    return SUCCESSFUL;
}

ThreadID Thread::id() const
{
    return m_threadID;
}

void Thread::yield()
{
    int rt = sched_yield();
    if ( rt < 0)
    {
        ERROR_LOG("Thread::yield sched_yield() error");
    }
}

void Thread::sleep( const Time& timeout )
{
    struct timeval tv = timeout;
    struct timespec ts;
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
    int rt = nanosleep( &ts, 0 );
    if ( rt < 0)
    {
    }
}

ThreadID::ThreadID( const pthread_t& tid ) :
    m_tid( tid )
{
}

ThreadID::ThreadID( const ThreadID& tid )
{
    *this = tid;
}

ThreadID& ThreadID::operator=( const ThreadID& tid )
{
    this->m_tid = tid.m_tid;
    return *this;
}

bool ThreadID::operator==( const ThreadID& tid )
{
    const int rt = pthread_equal( m_tid, tid.m_tid );

    if ( 0 == rt )  // not equal
    {
        return false;
    }

    return true;
}

bool ThreadID::operator!=( const ThreadID& tid )
{
    return !( *this == tid );
}

pthread_t ThreadID::getRawID() const
{
    return m_tid;
}


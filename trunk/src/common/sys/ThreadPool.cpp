/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.cpp
 *
 *    Description:  线程池实现
 *
 *        Version:  1.0
 *        Created:  04/27/2011 09:46:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "common/sys/ThreadPool.h"
#include "common/log/log.h"
#include "common/util/util.h"
using namespace std;

ThreadPool::EventHandlerThread::
EventHandlerThread()
{
    if ( NULL == g_pThreadPool )
    {
        DEBUG_LOG( "NULL pointer argument." );
    }
}

void ThreadPool::EventHandlerThread::
run()
{
    ThreadPoolWorkItem* pWorkItem = NULL;

    for ( ; ; )
    {
        // 取任务
        int rt = util::io::readn( g_pThreadPool->getRHandle(),
                                  static_cast<void*>( &pWorkItem ),
                                  sizeof( void* ) );
        pthread_testcancel();
        if ( -1 == rt )
        {
            if ( EINTR != errno )
            {
                DEBUG_LOG( "Syscall Error: read. %s", strerror( errno ) );
            }
        }

        // 处理任务
        pWorkItem->preProcess();
        pWorkItem->process();
        pWorkItem->postProcess();
    }

}

ThreadPool::ThreadPool( int maxIdle, int maxTotal ) :
    m_hReadHandle( -1 ),
    m_hWriteHandle( -1 ),
    m_nMaxIdle( maxIdle ),
    m_nMaxTotal( maxTotal )
{
    if ( maxIdle <= 0 ||
            maxTotal <= 0 ||
            maxIdle < maxTotal )
    {
        m_nMaxIdle = THREAD_MAX_IDLE_DEFAULT;
        m_nMaxTotal = THREAD_MAX_TOTAL_DEFAULT;
    }
}

int ThreadPool::getWHandle() const
{
    return m_hWriteHandle;
}

int ThreadPool::getRHandle() const
{
    return m_hReadHandle;
}

int ThreadPool::postRequest( 
    const ThreadPoolWorkItem* pWorkItem )
{
    int rt = util::io::writen( m_hWriteHandle, &pWorkItem, sizeof( void* ) );

    if ( -1 == rt )
    {
        return errno;
    }

    return SUCCESSFUL;
}

int ThreadPool::start()
{
    // init pipe
    int fd[2];

    int rt = pipe( fd );

    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pipe. %s", strerror( errno ) );
        return FAILED;
    }
    else
    {
        m_hReadHandle = fd[0];
        m_hWriteHandle = fd[1];
    }

    // create threads
    for ( int i = 0; i < m_nMaxIdle; ++i )
    {
        m_threadList.push_back( new EventHandlerThread() );
    }

    for ( std::list<Thread*>::iterator it = m_threadList.begin();
            it != m_threadList.end();
            ++it )
    {
        rt = ( *it )->start();
        if ( -1 == rt )
        {
            DEBUG_LOG( "Error occurred whilst starting a thread." );
            return FAILED;
        }
    }

    return SUCCESSFUL;
}

int ThreadPool::stop()
{
    INFO_LOG("threadpool stop");
    if ( !m_threadList.empty() )
    {
        for ( std::list<Thread*>::iterator it = m_threadList.begin();
                it != m_threadList.end();)
        {
            std::list<Thread*>::iterator tempit = it;
            ++it;
            delete ( *tempit );
            *tempit = NULL;
        }
    }
    
    return SUCCESSFUL;
}

ThreadPool::~ThreadPool()
{
    
}

int ThreadPool::ThreadExitWorkItem::
process()
{
    return SUCCESSFUL;
}


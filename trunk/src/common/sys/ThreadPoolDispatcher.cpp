/*
 * =====================================================================================
 *
 *       Filename:  ThreadPoolDispatcher.cpp
 *
 *    Description:  线程池任务转发
 *
 *        Version:  1.0
 *        Created:  04/27/2011 11:40:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <fcntl.h>

#include "common/sys/ThreadPoolDispatcher.h"
#include "common/sys/ThreadPool.h"
#include "common/log/log.h"
#include "common/comm/BaseTask.h"
#include "common/comm/TaskManager.h"
#include "common/comm/BaseHeader.h"
#include "common/util/util.h"
#include "common/comm/Epoll.h"

ThreadPoolDispatcher::ThreadPoolDispatcher()
{
    if ( NULL == g_pEpoll ||
            NULL == g_pThreadPool )
    {
        DEBUG_LOG(
            "NULL pointer argument passed to ThreadPoolDispatcher()." );
        return;
    }

    // set state
    m_iConnect = CONNECTED;

    // get thread pool handle
    m_hThreadPoolHandle = g_pThreadPool->getWHandle();

    int rt = setNonblock( m_hThreadPoolHandle );
    if ( rt < 0)
    {
    }

    // 设置线程池句柄
    m_writeEpollEvent.setFd( m_hThreadPoolHandle );
    rt = m_writeEpollEvent.registerWEvent();
    if ( rt < 0)
    {
    }
    rt = m_writeEpollEvent.closeWevent();
    if ( rt < 0)
    {
    }
    
    // init pipe
    int fd[2];
    rt = pipe( fd );
    if ( 0 != rt )
    {
        DEBUG_LOG( "Syscall Error: pipe. %s", strerror( errno ) );
    }

    m_hReadHandle = fd[0];
    m_hWriteHandle = fd[1];

    rt = setNonblock( m_hReadHandle );
    if ( rt < 0)
    {
    }

    m_readEpollEvent.setFd( m_hReadHandle );

    // 注册管道读事件
    rt = m_readEpollEvent.registerREvent();
    if ( rt < 0)
    {
    }
}

ThreadPoolDispatcher::~ThreadPoolDispatcher()
{
    try
    {
        int rt = m_readEpollEvent.unregisterRWEvents();
        if ( rt < 0)
        {
        }
        rt = m_writeEpollEvent.unregisterRWEvents();
        if ( rt < 0)
        {
        }
    }
    catch ( ... )
    {
    }
}

int
ThreadPoolDispatcher::init()
{
    m_readEpollEvent.setHandlerID( getID());
    m_writeEpollEvent.setHandlerID( getID() );
    return SUCCESSFUL;
}

int
ThreadPoolDispatcher::setNonblock( int fd ) const
{
    int val;

    if ( ( val = fcntl( fd, F_GETFL, 0 ) ) < 0 )
    {
        DEBUG_LOG( "Syscall Error: fcntl. %s", strerror( errno ) );
        return val;
    }

    val |= O_NONBLOCK;
    if ( fcntl( fd, F_SETFL, val ) < 0 )
    {
        DEBUG_LOG( "Syscall Error: fcntl. %s", strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}

void
ThreadPoolDispatcher::postRequest(
    ThreadPoolWorkItem* pWorkItem )
{
    m_workItemList.push_back( pWorkItem );

    // 打开线程池句柄写事件
    int rt = m_writeEpollEvent.openWevent();
    if ( rt < 0)
    {
    }
}

int
ThreadPoolDispatcher::sendData()
{
    int rt = 0;

    while ( !m_workItemList.empty() )
    {
        rt = g_pThreadPool->postRequest( m_workItemList.front() );
        if ( 0 != rt )
        {
            if ( EAGAIN != rt )
            {
                DEBUG_LOG(
                    "Error occurred while post request to thread pool." );
                return FAILED;
            }
            break;
        }
        else
        {
            m_workItemList.pop_front();
        }
    }

    if ( m_workItemList.empty() )
    {
        // 关闭写事件
        rt = m_writeEpollEvent.closeWevent();
        if ( rt < 0)
        {
        }
    }

    return SUCCESSFUL;
}

int
ThreadPoolDispatcher::recvData()
{
    ThreadPoolWorkItem* pWorkItem = NULL;

    int rt = 0;

    for ( ; ; )
    {
        rt = ::read( m_hReadHandle, &pWorkItem, sizeof( void* ) );

        if ( rt < 0 )
        {
            if ( EINTR == errno )
            {
                continue;
            }
            else if ( EAGAIN == errno )
            {
                break;
            }
            else
            {
                DEBUG_LOG( "Syscall Error: read. %s.", strerror( errno ) );
                break;
            }
        }

        BaseTask* pTask = TaskManager::getInstance()->get(
                              pWorkItem->getTaskID() );

        if ( NULL == pTask )
        {
            // task已析构
            delete pWorkItem;
            pWorkItem = NULL;
        }
        else
        {
            // task正常
            pTask->recvWorkItem( pWorkItem );
        }
    }

    return SUCCESSFUL;
}

void
ThreadPoolDispatcher::postResult(
    const ThreadPoolWorkItem* pWorkItem )
{
    int rt = util::io::writen( m_hWriteHandle, &pWorkItem, sizeof( void* ) );

    if ( -1 == rt )
    {
        DEBUG_LOG( "writen error: %s", strerror( errno ) );
    }
}


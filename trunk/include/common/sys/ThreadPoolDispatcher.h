/*
 * =====================================================================================
 *
 *       Filename:  ThreadPoolDispatcher.h
 *
 *    Description:  线程池任务转发
 *
 *        Version:  1.0
 *        Created:  04/27/2011 11:18:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#ifndef _THREADPOOL_DISPATCHER_H_
#define _THREADPOOL_DISPATCHER_H_

#include <list>

#include "common/comm/Agent.h"
#include "common/comm/EpollEvent.h"

class ThreadPoolWorkItem;

/**
 * @brief Dispatcher类自身维护一根管道，
 * 用于线程池中的任务将结果写回上层，
 * 此管道的读端注册到Epoll。
 * 另外，获取线程池管道的写端，将其注册到Epoll，
 * 这样可以非阻塞地将任务下发到线程池。
 */
class ThreadPoolDispatcher : public Agent
{
public:
    ThreadPoolDispatcher();
    ~ThreadPoolDispatcher();

	int init();

    /**
     * @brief 主线程内调用此函数，将任务放到此类的任务列表中,
     *        当ThreadPool中的管道触发可写事件时，会调用threapool
     *        的postRequest函数通过其管道将任务下发到线程池中。
     *
     * @param pWorkItem
     */
    void postRequest( ThreadPoolWorkItem* );

    /**
     * @brief 读事件到来，从管道读取处理结果
     *
     * @return
     */
    virtual int recvData();

    /**
     * @brief 写事件到来，将任务下发到线程池
     *
     * @return
     */
    virtual int sendData();

    /**
     * @brief 线程池内调用此函数,将任务回写到主线程
     *
     * @return
     */
    void postResult( const ThreadPoolWorkItem* );

protected:
    /**
     * @brief 将描述符设为非阻塞
     *
     * @param fd 描述符
     *
     * @return
     */
    int setNonblock( int fd ) const;

protected:
    int m_hThreadPoolHandle;
    int m_hReadHandle;
    int m_hWriteHandle;
    std::list<ThreadPoolWorkItem*> m_workItemList;

    // for epoll
    EpollEvent m_readEpollEvent;
    EpollEvent m_writeEpollEvent;
};
extern ThreadPoolDispatcher* g_pDispatcher;

#endif  // _THREADPOOL_DISPATCHER_H_


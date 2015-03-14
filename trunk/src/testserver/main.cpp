/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/13 05:07:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "ClientAgent.h"

#include "common/comm/Epoll.h"
#include "common/comm/TCPListenAgent.h"
#include "common/comm/AgentManager.h"
#include "common/comm/TaskManager.h"
#include "common/sys/ThreadPool.h"
#include "common/log/log.h"
#include "common/util/util.h"

#include <stdlib.h>
#include <signal.h>

using util::conv::conv;

Epoll  *g_pEpoll  = NULL;
TCPListenAgent<ClientAgent> *g_pListenCA = NULL;
ThreadPool *g_pThreadPool = NULL;
ThreadPoolDispatcher *g_pDispatcher = NULL;
SocketAddress servaddr;

const int EPOLLSIZE = 1024;

void doExit(int signo)
{
    if (signo == SIGINT)       
    {
        INFO_LOG("server sigint catch!");
        
        if ( g_pEpoll != NULL)
        {
            g_pEpoll->finish();
        }
    }
}

int main(int argc, char *argv[])
{
    if ( argc != 2 )
    {
        cout << "Usage: " << argv[0] << " Port" << endl;
        return FAILED;
    }

    g_pEpoll = new Epoll();
    if (g_pEpoll->initialize(EPOLLSIZE) == FAILED)
    {
        ERROR_LOG("\nIn main: init epoll error!\n");
        delete g_pEpoll;
        g_pEpoll = NULL;
        return FAILED;
    }

    g_pThreadPool = new ThreadPool();
    g_pThreadPool->start();

    //must init epoll and thread pool before
    g_pDispatcher = (AgentManager::getInstance())->createAgent<ThreadPoolDispatcher>();
    g_pDispatcher->init();
    
    servaddr.setPort(conv<unsigned short,char*>(argv[1]));
    g_pListenCA = (AgentManager::getInstance())->createAgent< TCPListenAgent<ClientAgent> >(servaddr);
    g_pListenCA->init();
    
    if (signal(SIGINT, doExit) == SIG_ERR)
    {
        ERROR_LOG("\nIn main: set SIGINT(catch \"ctrl+c\") signal error!\n");
        return FAILED;
    }
    g_pEpoll->run();
   
    AgentManager::destroyInstance();
    TaskManager::destroyInstance();    

    g_pThreadPool->stop();
    if ( g_pThreadPool != NULL)
    {
        delete g_pThreadPool;
        g_pThreadPool = NULL;
    }
    
    if ( g_pEpoll != NULL)
    {
        delete g_pEpoll;
        g_pEpoll = NULL;
    }

    LoggerFactory::getInstance()->clear();
    LoggerFactory::destroyInstance();
    
    return 0;
}

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

#include "ServerAgent.h"
#include "common/comm/Epoll.h"
#include "common/comm/TCPListenAgent.h"
#include "common/comm/AgentManager.h"
#include "common/comm/TaskManager.h"
#include "common/log/log.h"
#include "common/util/util.h"

#include <stdlib.h>
#include <signal.h>
#include <cstdio>
using util::conv::conv;

Epoll  *g_pEpoll  = NULL;
ServerAgent *g_pServerAgent = NULL;
SocketAddress servaddr;

const int EPOLLSIZE = 1024;

void doExit(int signo)
{
    if (signo == SIGINT)       
    {
        INFO_LOG("client sigint catch!");
        
        AgentManager::getInstance()->clearAllAgent();
        TaskManager::getInstance()->clearAllTask();
        
        if ( g_pEpoll != NULL)
        {
            g_pEpoll->finish();
        }     
    }
}

int main(int argc, char *argv[])
{
    if ( argc != 3 )
    {
        cout << "Usage: " << argv[0] << " IP Port" << endl;
        return FAILED;
    }

    g_pEpoll = new Epoll();
    if ( g_pEpoll->initialize(EPOLLSIZE) == FAILED )
    {
        ERROR_LOG("\nIn main: init epoll error!\n");
        delete g_pEpoll;
        g_pEpoll = NULL;
        return FAILED;
    }
    
    servaddr.setAddress(argv[1],conv<unsigned short,char*>(argv[2]));
    g_pServerAgent = (AgentManager::getInstance())->createAgent<ServerAgent>(servaddr);
    g_pServerAgent->init();

    if ( signal(SIGINT, doExit) == SIG_ERR )
    {
        ERROR_LOG("\nIn main: set SIGINT(catch \"ctrl+c\") signal error!\n");
        return FAILED;
    }
    
    g_pEpoll->run();

    AgentManager::destroyInstance();
    TaskManager::destroyInstance();
    
    if ( g_pEpoll != NULL )
    {
        delete g_pEpoll;
        g_pEpoll = NULL;
    }

    LoggerFactory::getInstance()->clear();
    LoggerFactory::destroyInstance();
    
    return 0;
}

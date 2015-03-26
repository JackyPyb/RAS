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
#include "ConfigManager.h"

#include <stdlib.h>
#include <signal.h>
#include <cstdio>
#include <unistd.h>
using util::conv::conv;

Epoll  *g_pEpoll  = NULL;
ServerAgent *g_pServerAgent = NULL;
SocketAddress servaddr;
char * const shortOpt = "hf:";

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
        cout << "Usage: " << argv[0] << " -f config_filename" << endl;
        return FAILED;
    }

    int ch = 0;
    const char *pConfigFileName = NULL;
    while((ch = getopt(argc, argv, shortOpt)) != -1)
    {
        switch(ch)
        {
            case 'h':
                std::cout << "Usage: " << argv[0] 
                    << " -f config_filename" << std::endl;
                return SUCCESSFUL;
            case 'f':
                pConfigFileName = optarg;
                break;
            default:
                std::cerr << "Unknown option: " << char(optopt) << std::endl;
                return FAILED;
        }
    }

    if(NULL == pConfigFileName)
        pConfigFileName = "config.xml";

    if(FAILED == (ConfigManager::getInstance())->configWithXML(pConfigFileName))
    {
        ERROR_LOG("AL : Config init ERROR");
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
    
    servaddr.setAddress(
            (ConfigManager::getInstance())->getConnectIP().c_str(),
            (ConfigManager::getInstance())->getConnectPort());
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

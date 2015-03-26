#include "ConfigManager.h"
#include "RunControl.h"
#include "head.h"

#include "common/comm/TaskManager.h"
#include "common/comm/AgentManager.h"
#include "common/log/log.h"
#include "common/comm/Epoll.h"
#include "common/Timer/Timer.h"
#include "common/comm/Error.h"

#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>

using namespace rc;

char * const shortOpt = "hf:";
TimerManager *g_pTimerManager = NULL;
Epoll *g_pEpoll = NULL;

void sig_internal(int sigNo);

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] 
            << " -f config_filename" << std::endl;
        exit(FAILED);
    }

    if(SIG_ERR == (signal(SIGINT, sig_internal)))
    {
        ERROR_LOG("Can not catch SIGINT");
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
        pConfigFileName = "RASRC_config.xml";

    if(FAILED == (ConfigManager::getInstance())->configWithXML(pConfigFileName))
    {
        ERROR_LOG("RC : Config init ERROR");
        return FAILED;
    }

    #ifdef DEBUG
    INFO_LOG("Epoll Max Fd is %d", 
            (ConfigManager::getInstance())->getEpollMaxFd());
    INFO_LOG("NC listen port is %d",
            ((ConfigManager::getInstance())->getNCListenAddr()).getPort());
    INFO_LOG("SAS listen port is %d",
            ((ConfigManager::getInstance())->getSASListenAddr()).getPort());
    INFO_LOG("FWM listen port is %d",
            ((ConfigManager::getInstance())->getFWMListenAddr()).getPort());
    INFO_LOG("AL listen port is %d",
            ((ConfigManager::getInstance())->getALListenAddr()).getPort());
    INFO_LOG("NC heart beat time out is %d",
            (ConfigManager::getInstance())->getNCHeartBeatTimeOut());
    INFO_LOG("NC heart beat retry num is %d",
            (ConfigManager::getInstance())->getNCHeartBeatRetryNum());
    INFO_LOG("FWM heart beat time out is %d",
            (ConfigManager::getInstance())->getFWMRootHeartBeatTimeOut());
    INFO_LOG("FWM heart beat retry num is %d",
            (ConfigManager::getInstance())->getFWMRootHeartBeatRetryNum());
    INFO_LOG("Min NC num is %d",
            (ConfigManager::getInstance())->getMinNCNum());
    INFO_LOG("Local IP is %s",
            (ConfigManager::getInstance())->getLocalIP().c_str());
    #endif

    g_pTimerManager = (AgentManager::getInstance()->
            createAgent<TimerManager>());
    if(g_pTimerManager == NULL)
    {
        ERROR_LOG("main: can not init TimerManager");
        return FAILED;
    }

    g_pTimerManager->init();

//    TaskManager::getInstance()->setIDGenerator(GeneralIDGenerator());

    (RunControl::getInstance())->run();

    (RunControl::getInstance())->epollRun();

    AgentManager::destroyInstance();
    TaskManager::destroyInstance();
    if(g_pEpoll != NULL)
    {
        delete g_pEpoll;
        g_pEpoll = NULL;
    }

    LoggerFactory::getInstance()->clear();
    LoggerFactory::destroyInstance();
    return 0;
}

void sig_internal(int sigNo)
{
    if(sigNo == SIGINT)
    {
        INFO_LOG("Catch SIGINT");
        if(g_pEpoll != NULL)
        {
            g_pEpoll->finish();
        }
    }
    exit(FAILED);
}

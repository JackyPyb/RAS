#include "RunControl.h"
#include "ConfigManager.h"
#include "StrategyManager.h"
//#include "NCAgent.h"
//#include "SASAgent.h"

#include "common/comm/TCPListenAgent.h"
#include "common/comm/AgentManager.h"
#include "common/comm/Epoll.h"
#include "common/log/log.h"

extern Epoll *g_pEpoll;

namespace rc
{

RunControl::RunControl()
{

}

RunControl::~RunControl()
{

}

void RunControl::run(void)
{
    g_pEpoll = new Epoll();
    if(g_pEpoll->initialize(
                (ConfigManager::getInstance())->getEpollMaxFd()) < 0)
    {
        ERROR_LOG("g_pEpoll Init Error");
        return;
    }

    this->runRC();
}

void RunControl::runRC(void)
{
    INFO_LOG("************* RC Run ************");
   (StrategyManager::getInstance())->init();
   this->NCListen();
   this->SASListen();
}

void RunControl::NCListen(void)
{
//    TCPListenAgent<NCAgent> *pNCListenAgent = 
//        (AgentManager::getInstance())
//        ->createAgent< TCPListenAgent<NCAgent> >(g_pEpoll);
//    SocketAddress NCListenAddress = 
//        (ConfigManager::getInstance())->getNCListenAddr();
//    pNCListenAgent->init(NCListenAddress);
    INFO_LOG("Listen to NC");
}

void RunControl::SASListen(void)
{
//    TCPListenAgent<SASAgent> *pSASListenAgent = 
//        (AgentManager::getInstance())
//        ->createAgent< TCPListenAgent<SASAgent> >(g_pEpoll);
//    SocketAddress SASListenAddress = 
//        (ConfigManager::getInstance())->getSASListenAddr();
//    pSASListenAgent->init(SASListenAddress);
    INFO_LOG("Listen to SAS");
}

void RunControl::epollRun(void)
{
    g_pEpoll->run();
}

}

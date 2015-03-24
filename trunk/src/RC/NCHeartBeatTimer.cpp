#include "NCHeartBeatTimer.h"
#include "ConfigManager.h"
#include "ResourceManager.h"

#include "common/comm/AgentManager.h"
#include "common/log/log.h"

namespace rc
{

NCHeartBeatTimer::NCHeartBeatTimer(
        unsigned int timeLen, uint32_t aid, string ncip):
        Timer(timeLen),
        m_NCAgentID(aid),
        m_NCIP(ncip)
{
    m_retryNum = (ConfigManager::getInstance())->getNCHeartBeatRetryNum();
    ERROR_LOG("NCHeartBeatTimer::NCHeartBeatTimer: m_retryNum is %d", 
            m_retryNum);
}

NCHeartBeatTimer::~NCHeartBeatTimer()
{
    ERROR_LOG("NCHeartBeatTimer::~NCHeartBeatTimer");
}

int NCHeartBeatTimer::doAction()
{
    ERROR_LOG("NCHeartBeatTimer::doAction");
    if(m_retryNum > 0)
    {
        ERROR_LOG("NCHeartBeatTimer::doAction: m_retryNum is %d", m_retryNum);
        m_retryNum--;
            updateExpiredTime(
                    (ConfigManager::getInstance())->getNCHeartBeatTimeOut());
    }
    else
    {
        ERROR_LOG(
                "NCHeartBeatTimer::doAction: heartbeat timeout!");
        if((ResourceManager::getInstance())->dealNCCrash(m_NCIP) < 0)
        {
            return SUCCESSFUL;
        }
        (AgentManager::getInstance())->recycle(m_NCAgentID);
        m_NCIP.clear();
    }

    return SUCCESSFUL;
}

void NCHeartBeatTimer::setRetryNum(unsigned int num)
{
    m_retryNum = num;
}

unsigned int NCHeartBeatTimer::getRetryNum() const
{
    return m_retryNum;
}

void NCHeartBeatTimer::resetRetryNum()
{
    m_retryNum = (ConfigManager::getInstance())->getNCHeartBeatRetryNum();
}

}

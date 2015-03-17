#include "FWMHeartBeatTimer.h"
#include "ConfigManager.h"
#include "ResourceManager.h"

#include "common/comm/AgentManager.h"
#include "common/log/log.h"

namespace rc
{

FWMHeartBeatTimer::FWMHeartBeatTimer(
        unsigned int timeLen, uint32_t aid, 
        string ncip, uint32_t frameworkID, uint32_t fwInstanceID):
        Timer(timeLen),
        m_FWMAgentID(aid),
        m_FWMIP(ncip),
        m_frameworkID(frameworkID),
        m_fwInstanceID(fwInstanceID)
{
    m_retryNum = (ConfigManager::getInstance())->getFWMRootHeartBeatRetryNum();
}

FWMHeartBeatTimer::~FWMHeartBeatTimer()
{

}

int FWMHeartBeatTimer::doAction()
{
    if(m_retryNum > 0)
    {
        m_retryNum--;
            updateExpiredTime(
                    (ConfigManager::getInstance())->getFWMRootHeartBeatTimeOut());
    }
    else
    {
        ERROR_LOG(
                "FWMHeartBeatTimer::doAction: heartbeat timeout!");
        (AgentManager::getInstance())->recycle(m_FWMAgentID);
        m_FWMIP.clear();
    }

    return SUCCESSFUL;
}

void FWMHeartBeatTimer::setRetryNum(unsigned int num)
{
    m_retryNum = num;
}

unsigned int FWMHeartBeatTimer::getRetryNum() const
{
    return m_retryNum;
}

void FWMHeartBeatTimer::resetRetryNum()
{
    m_retryNum = (ConfigManager::getInstance())->getFWMRootHeartBeatRetryNum();
}

}

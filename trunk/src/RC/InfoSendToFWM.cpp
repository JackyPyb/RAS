#include "InfoSendToFWM.h"
#include "FWMAgent.h"

#include "common/comm/AgentManager.h"
#include "protocol/RASCmdCode.h"
#include "common/log/log.h"

namespace rc
{

class FWMAgent;

InfoSendToFWM::InfoSendToFWM(uint32_t aid):
    m_FWMAgentID(aid),
    m_data("")
{
 
}

InfoSendToFWM::~InfoSendToFWM()
{
    m_data.clear();
}

int InfoSendToFWM::sendTaskToFWM(Task *pTask)
{
    uint32_t taskType = pTask->getTaskType();
    switch(taskType)
    {

    }

    FWMAgent *pAgent = dynamic_cast<FWMAgent*>(
            (AgentManager::getInstance())->get(m_FWMAgentID));
    if(pAgent == NULL)
    {
        ERROR_LOG("InfoSendToFWM::sendTaskToFWM: ncagent can not found");
        return FAILED;
    }
    return pAgent->sendPackage(m_msg, m_data);
}

void InfoSendToFWM::setMsgHeader(uint32_t cmd, uint32_t len, uint64_t tid)
{
    m_msg.cmd = cmd;
    m_msg.length = len;
    m_msg.para1 = tid;
    m_msg.para2 = tid >> 32;
}

}

#include "InfoSendToNC.h"
#include "NCAgent.h"

#include "common/comm/AgentManager.h"
#include "protocol/RASCmdCode.h"
#include "common/log/log.h"

namespace rc
{

class NCAgent;

InfoSendToNC::InfoSendToNC(uint32_t aid):
    m_NCAgentID(aid),
    m_data("")
{
 
}

InfoSendToNC::~InfoSendToNC()
{
    m_data.clear();
}

int InfoSendToNC::sendTaskToNC(Task *pTask)
{
    uint32_t taskType = pTask->getTaskType();
    switch(taskType)
    {

    }

    NCAgent *pAgent = dynamic_cast<NCAgent*>(
            (AgentManager::getInstance())->get(m_NCAgentID));
    if(pAgent == NULL)
    {
        ERROR_LOG("InfoSendToNC::sendTaskToNC: ncagent can not found");
        return FAILED;
    }
    return pAgent->sendPackage(m_msg, m_data);
}

void InfoSendToNC::setMsgHeader(uint32_t cmd, uint32_t len, uint64_t tid)
{
    m_msg.cmd = cmd;
    m_msg.length = len;
    m_msg.para1 = tid;
    m_msg.para2 = tid >> 32;
}

}

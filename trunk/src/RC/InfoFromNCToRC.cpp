#include "InfoFromNCToRC.h"
#include "NCAgent.h"

#include "common/log/log.h"
#include "common/comm/AgentManager.h"

namespace rc
{

InfoFromNCToRC::InfoFromNCToRC(uint32_t aid, string ip):
    m_NCAgentID(aid),
    m_NCIP(ip)
{

}

InfoFromNCToRC::~InfoFromNCToRC()
{
    m_NCIP.clear();
}

int InfoFromNCToRC::handleNCReq(InReq &req)
{
    int ret = 0;
    uint64_t taskID = mergeID(req.m_msgHeader.para1, req.m_msgHeader.para2);
    switch(req.m_msgHeader.cmd)
    {
        
    }

    return ret;
}

int InfoFromNCToRC::taskReportFromNC(
        uint32_t result, uint64_t tid)
{
    return 0;
}

int InfoFromNCToRC::doRegister(uint64_t tid, string str)
{

}

void InfoFromNCToRC::sendAckToNC(
        uint32_t cmd, uint32_t err, uint32_t low, uint32_t high)
{
    MsgHeader msg;
    msg.cmd = cmd;
    msg.length = 0;
    msg.error = err;
    msg.para1 = low;
    msg.para2 = high;

    NCAgent *pAgent = dynamic_cast<NCAgent*>(
            (AgentManager::getInstance())->get(m_NCAgentID));
    if(pAgent == NULL)
    {
        ERROR_LOG("InfoFromNCToRC::sendAckToNC: ncagent can not found");
        return;
    }
    pAgent->sendPackage(msg);
}

uint64_t InfoFromNCToRC::mergeID(uint32_t low, uint32_t high) const
{
    uint64_t temp = (uint64_t)high << 32;
    temp += (uint64_t)low;
    return temp;
}

}

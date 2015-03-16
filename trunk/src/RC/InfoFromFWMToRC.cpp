#include "InfoFromFWMToRC.h"
#include "FWMAgent.h"

#include "common/log/log.h"
#include "common/comm/AgentManager.h"

namespace rc
{

InfoFromFWMToRC::InfoFromFWMToRC(uint32_t aid, string ip, 
        uint32_t fwid, uint32_t fwInstanceID):
    m_FWMAgentID(aid),
    m_FWMIP(ip),
    m_frameworkID(fwid),
    m_fwInstanceID(fwInstanceID)
{

}

InfoFromFWMToRC::~InfoFromFWMToRC()
{
    m_FWMIP.clear();
}

int InfoFromFWMToRC::handleFWMReq(InReq &req)
{
    int ret = 0;
    uint64_t taskID = mergeID(req.m_msgHeader.para1, req.m_msgHeader.para2);
    switch(req.m_msgHeader.cmd)
    {
        
    }

    return ret;
}

int InfoFromFWMToRC::taskReportFromFWM(
        uint32_t result, uint64_t tid)
{
    return 0;
}

int InfoFromFWMToRC::doRegister(uint64_t tid, string str)
{

}

void InfoFromFWMToRC::sendAckToFWM(
        uint32_t cmd, uint32_t err, uint32_t low, uint32_t high)
{
    MsgHeader msg;
    msg.cmd = cmd;
    msg.length = 0;
    msg.error = err;
    msg.para1 = low;
    msg.para2 = high;

    FWMAgent *pAgent = dynamic_cast<FWMAgent*>(
            (AgentManager::getInstance())->get(m_FWMAgentID));
    if(pAgent == NULL)
    {
        ERROR_LOG("InfoFromFWMToRC::sendAckToFWM: ncagent can not found");
        return;
    }
    pAgent->sendPackage(msg);
}

uint64_t InfoFromFWMToRC::mergeID(uint32_t low, uint32_t high) const
{
    uint64_t temp = (uint64_t)high << 32;
    temp += (uint64_t)low;
    return temp;
}

}

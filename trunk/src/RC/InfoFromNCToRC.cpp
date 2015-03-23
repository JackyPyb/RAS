#include "InfoFromNCToRC.h"
#include "NCAgent.h"
#include "NCRegTask.h"
#include "ConfigManager.h"
#include "NCHeartBeatTask.h"
#include "StartFWRootTask.h"
#include "head.h"

#include "common/log/log.h"
#include "common/comm/AgentManager.h"
#include "common/comm/TaskManager.h"
#include "protocol/RASCmdCode.h"

extern Epoll *g_pEpoll;

namespace rc
{

InfoFromNCToRC::InfoFromNCToRC(uint32_t aid, string ip):
    m_NCAgentID(aid),
    m_NCIP(ip),
    m_pNCHeartBeatTimer(NULL)
{

}

InfoFromNCToRC::~InfoFromNCToRC()
{
    m_NCIP.clear();
    if(m_pNCHeartBeatTimer != NULL)
    {
        m_pNCHeartBeatTimer->setRetryNum(0);
        m_pNCHeartBeatTimer->doAction();
        m_pNCHeartBeatTimer->detachTimer();
    }
}

int InfoFromNCToRC::handleNCReq(InReq &req)
{
    int ret = 0;
    uint64_t taskID = mergeID(req.m_msgHeader.para1, req.m_msgHeader.para2);
    switch(req.m_msgHeader.cmd)
    {
        case MSG_NC_RC_REGISTER:
        {
            #ifdef DEBUG
            INFO_LOG("InfoFromNCToRC::handleNCReq: MSG_NC_RC_REGISTER");
            #endif

            string data(req.ioBuf, req.m_msgHeader.length);
            ret = doRegister(taskID, data);
            sendAckToNC(
                    MSG_NC_RC_REGISTER_ACK,
                    ret,
                    req.m_msgHeader.para1,
                    req.m_msgHeader.para2);
            break;
        }
        case MSG_NC_RC_SEND_HEARTBEAT_AND_MONITOR_INFOMATION:
        {
            #ifdef DEBUG
            INFO_LOG("InfoFromNCToRC::handleNCReq: \
                    MSG_NC_RC_SEND_HEARTBEAT_AND_MONITOR_INFOMATION");
            #endif

            if(m_pNCHeartBeatTimer == NULL)
            {
                m_pNCHeartBeatTimer = new NCHeartBeatTimer(
                        (ConfigManager::getInstance())->getNCHeartBeatTimeOut(), 
                        m_NCAgentID, m_NCIP);
                m_pNCHeartBeatTimer->attachTimer();
            }
            else
            {
                m_pNCHeartBeatTimer->updateExpiredTime(
                        (ConfigManager::getInstance())->getNCHeartBeatTimeOut());
                m_pNCHeartBeatTimer->resetRetryNum();
            }

            sendAckToNC(
                    MSG_NC_RC_SEND_HEARTBEAT_AND_MONITOR_INFOMATION_ACK,
                    0,
                    req.m_msgHeader.para1,
                    req.m_msgHeader.para2);

            string data(req.ioBuf, req.m_msgHeader.length);
            NCHeartBeatTask *pTask = 
                (TaskManager::getInstance())->create<NCHeartBeatTask>();
            pTask->setNCIP(m_NCIP);
            pTask->setDataString(data);
            pTask->goNext();
            break;            
        }
        case MSG_RC_NC_START_FRAMEWORK_ROOT_ACK:
        {
#ifdef DEBUG
            INFO_LOG("InfoFromNCToRC start root ack!");
#endif
            StartFWRootTask *pStartRootTask = 
                dynamic_cast<StartFWRootTask*>((TaskManager::getInstance())
                    ->get(taskID));
            if(pStartRootTask == NULL)
            {
                ERROR_LOG(
                        "InfoFromNCToRC::handleNCReq: StartFWRootTask not found!");
                return FAILED;
            }
            string data(req.ioBuf,req.m_msgHeader.length);
            pStartRootTask->setDataString(data);
            pStartRootTask->goNext();
            break;            
        }
        default:
            ERROR_LOG(
                    "InfoFromNCToRC::handleNCReq: \
                    Invaild MSG_HEADER_CMD");
    }

    return ret;
}

int InfoFromNCToRC::taskReportFromNC(
        uint32_t result, uint64_t tid)
{
    return 0;
}

int InfoFromNCToRC::doRegister(uint64_t tid, const string &str)
{
    NCRegTask *pTask = (TaskManager::getInstance())->create<NCRegTask>();
    pTask->setAgentID(m_NCAgentID);
    pTask->setDataString(str);
    pTask->setNCTaskID(tid);
    return pTask->goNext();    
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
    
    #ifdef DEBUG
    INFO_LOG("send ack message to NC: cmd is %x", cmd);
    #endif
}

uint64_t InfoFromNCToRC::mergeID(uint32_t low, uint32_t high) const
{
    uint64_t temp = (uint64_t)high << 32;
    temp += (uint64_t)low;
    return temp;
}

}

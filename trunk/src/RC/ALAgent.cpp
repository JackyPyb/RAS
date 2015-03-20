#include "ALAgent.h"
#include "StartFWRootTask.h"
#include "protocol/RASCmdCode.h"

#include "common/comm/TaskManager.h"
#include "common/log/log.h"

namespace rc
{

ALAgent::ALAgent(const TCPSocket &sock,
        const SocketAddress &oppoAddr):
    TCPAgent(sock, oppoAddr)
{

}

ALAgent::~ALAgent()
{

}

void ALAgent::readBack(InReq &req)
{
    uint64_t taskID = mergeID(req.m_msgHeader.para1, req.m_msgHeader.para2);
    
    switch(req.m_msgHeader.cmd)
    {
        case MSG_AL_RC_START_ROOT_MODULE:
        {
            string data(req.ioBuf, req.m_msgHeader.length);
            StartFWRootTask *pTask = 
                (TaskManager::getInstance())->create<StartFWRootTask>();
            pTask->setDataString(data);
            pTask->setALTaskID(taskID);
            pTask->setAgentID(getID());
            pTask->goNext();
            break;            
        }
    }
}

void ALAgent::setMsgHeader(
    uint32_t cmd, uint32_t err, uint32_t len, uint64_t tid)
{
    m_msg.cmd = cmd;
    m_msg.length = len;
    m_msg.error = err;
    m_msg.para1 = tid;
    m_msg.para2 = tid >> 32;
}

uint64_t ALAgent::mergeID(uint32_t low, uint32_t high) const
{
    uint64_t temp = (uint64_t)high << 32;
    temp += (uint64_t)low;
    return temp;
}

int ALAgent::sendPackage(MsgHeader &msg, string data)
{
    uint32_t buffLen = HEADER_SIZE + msg.length;
    char *buff = new char[buffLen + 1];
    memset(buff, 0, buffLen + 1);
    memcpy(buff, &msg, HEADER_SIZE);
    if(msg.length != 0)
        memcpy(buff + HEADER_SIZE, data.c_str(), msg.length);

    if(TCPAgent::writeToBuffer(buff, buffLen) < 0)
    {
        ERROR_LOG("NCAgent::sendPackage : writeToBuffer Error");
        if(buff != NULL)
        {
            delete [] buff;
            buff = NULL;
        }
        return FAILED;
    }

    return SUCCESSFUL;
}

void ALAgent::recycler(void)
{
    TCPAgent::recycler();
}

}

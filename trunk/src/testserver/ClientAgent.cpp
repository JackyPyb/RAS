/*
 * =====================================================================================
 *
 *       Filename:  ClientAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/05/13 04:19:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "ClientAgent.h"
#include "testprotocol.h"
#include "TestTask.h"

#include "common/comm/TaskManager.h"
#include "common/log/log.h"

ClientAgent::ClientAgent()
{

}

ClientAgent::ClientAgent(const SocketAddress &addr):
    TCPAgent(addr)
{

}

ClientAgent::ClientAgent(const TCPSocket &sock, const SocketAddress &addr):
    TCPAgent(sock,addr)
{

}

ClientAgent::~ClientAgent()
{
}

void
ClientAgent::readBack(InReq &req)
{
    switch (req.m_msgHeader.cmd)
    {
        case MSG_C_S_INFO:
        {
            string data(req.ioBuf, req.m_msgHeader.length);
            TestTask *pTask = 
                TaskManager::getInstance()->create<TestTask>();
            pTask->setNum(req.m_msgHeader.para1);
            pTask->setData(data);
            pTask->goNext();
            
            MsgHeader msg;
            msg.cmd = MSG_C_S_INFO_ACK;
            msg.length = req.m_msgHeader.length;
            msg.para1 = req.m_msgHeader.para1;
            sendPackage(msg, req.ioBuf);
            data.clear();
            break;
        }
        default:
            break;
    }
}

void
ClientAgent::writeBack(bool result)
{
    if (!result)
    {
        ERROR_LOG("\nIn ClientAgent: "
                "writeBack(): write out error!\n");
    }
}

int
ClientAgent::sendPackage(MsgHeader &header, const char *dataStr)
{
    uint32_t msgLength = header.length + sizeof(MsgHeader);
    char *sendBuf = new char[msgLength + 1];
    memset(sendBuf, 0, msgLength + 1);
    memcpy(sendBuf, &header, sizeof(MsgHeader));
    if (dataStr != NULL)
    {
        memcpy(sendBuf + sizeof(MsgHeader), dataStr, header.length);
    }

    if (this->writeToBuffer(sendBuf, msgLength) == FAILED)
    {
        DEBUG_LOG("\nIn ClientAgent: "
                "sendPackage(): write data error!\n");
        return FAILED;
    }

    return SUCCESSFUL;
}

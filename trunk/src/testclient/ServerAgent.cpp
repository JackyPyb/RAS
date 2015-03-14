/*
 * =====================================================================================
 *
 *       Filename:  ServerAgent.cpp
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

#include "ServerAgent.h"
#include "testprotocol.h"

#include "common/comm/Epoll.h"
#include "common/comm/SocketAddress.h"
#include "common/log/log.h"

const unsigned int buflen = 1024;

ServerAgent::ServerAgent():
    m_num(0)
{

}

ServerAgent::ServerAgent(const SocketAddress &addr):
    TCPAgent(addr),
    m_num(0)
{

}

ServerAgent::ServerAgent(const TCPSocket &sock, const SocketAddress &addr):
    TCPAgent(sock,addr),
    m_num(0)
{

}

ServerAgent::~ServerAgent()
{
}

int
ServerAgent::init()
{
    if ( this->connect() != SUCCESSFUL )
    {
        ERROR_LOG("in ServerAgent::init connect server error");
        return FAILED;
    }

    return SUCCESSFUL;
}

int
ServerAgent::connectAfter( bool bConnect)
{
    if (bConnect )
    {
        m_num++;
        MsgHeader msg;
        msg.cmd = MSG_C_S_INFO;
        msg.length = buflen;
        msg.para1 = m_num;

        char* buf = new char[buflen + 1];
        memset(buf,0,buflen + 1);
        memset(buf,'d',buflen);

        sendPackage(msg,buf);

        delete []buf;
        buf = NULL;

        return SUCCESSFUL;
    }
    else
    {
        return FAILED;
    }
}
void
ServerAgent::readBack(InReq &req)
{
    switch (req.m_msgHeader.cmd)
    {
        case MSG_C_S_INFO_ACK:
        {
            m_num++;
            MsgHeader msg;
            msg.cmd = MSG_C_S_INFO;
            msg.length = req.m_msgHeader.length;
            msg.para1 = m_num;
            sendPackage(msg, req.ioBuf);
            break;
        }
        default:
            break;
    }
}

void
ServerAgent::writeBack(bool result)
{
    if (!result)
    {
        ERROR_LOG("\nIn ServerAgent: "
                "writeBack(): write out error!\n");
    }
}

int
ServerAgent::sendPackage(MsgHeader &header, const char *dataStr)
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
        DEBUG_LOG("\nIn ServerAgent: "
                "sendPackage(): write data error!\n");
        return FAILED;
    }

    return SUCCESSFUL;
}

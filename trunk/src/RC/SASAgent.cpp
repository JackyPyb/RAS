#include "SASAgent.h"

#include "common/log/log.h"

namespace rc
{

SASAgent::SASAgent(const TCPSocket &sock,
        const SocketAddress &oppoAddr):
    TCPAgent(sock, oppoAddr)
{

}

SASAgent::~SASAgent()
{

}

void SASAgent::readBack(InReq &req)
{

}

void SASAgent::setMsgHeader(
    uint32_t cmd, uint32_t err, uint32_t len, uint64_t tid)
{
    m_msg.cmd = cmd;
    m_msg.length = len;
    m_msg.error = err;
    m_msg.para1 = tid;
    m_msg.para2 = tid >> 32;
}


int SASAgent::sendPackage(MsgHeader &msg, string data)
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

void SASAgent::recycler(void)
{
    TCPAgent::recycler();
}

}

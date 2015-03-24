#include "NCAgent.h"
#include "head.h"

#include "common/log/log.h"

namespace rc
{

NCAgent::NCAgent(const TCPSocket &sock,
        const SocketAddress &oppoAddr):
    TCPAgent(sock, oppoAddr)
{
    m_NCIP = string(oppoAddr.getIP());        

    #ifdef DEBUG
    INFO_LOG("NCAgent construction");
    INFO_LOG("NC IP is %s", m_NCIP.c_str());
    #endif
}

NCAgent::~NCAgent()
{
#ifdef DEBUG
    INFO_LOG("NCAgent::~NCAgent");
#endif
    if(m_pInfoFromNCToRC != NULL)
    {
        delete m_pInfoFromNCToRC;
        m_pInfoFromNCToRC = NULL;
    }
}

int NCAgent::init(void)
{
    TCPAgent::init();
#ifdef DEBUG
    INFO_LOG("NCAgent::init");
#endif
    m_pInfoFromNCToRC = new InfoFromNCToRC(getID(), m_NCIP);
    return SUCCESSFUL;
}

void NCAgent::readBack(InReq &req)
{
#ifndef DEBUG
    INFO_LOG("NCAgent::readBack");
#endif

    int ret = m_pInfoFromNCToRC->handleNCReq(req);
    if(ret < 0)
    {
        ERROR_LOG("NCAgent::readBack : ret < 0");
    }
}

void NCAgent::setMsgHeader(
    uint32_t cmd, uint32_t err, uint32_t len, uint64_t tid)
{
    m_msg.cmd = cmd;
    m_msg.length = len;
    m_msg.error = err;
    m_msg.para1 = tid;
    m_msg.para2 = tid >> 32;
}


int NCAgent::sendPackage(MsgHeader &msg, string data)
{
#ifdef DEBUG
    INFO_LOG("NCAgent:: sendPackage: cmd is %d", msg.cmd);
#endif
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

void NCAgent::recycler(void)
{
    TCPAgent::recycler();
}

}

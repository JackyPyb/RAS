#include "FWMAgent.h"

#include "common/log/log.h"

namespace rc
{

FWMAgent::FWMAgent(const TCPSocket &sock,
        const SocketAddress &oppoAddr):
    TCPAgent(sock, oppoAddr)
{
    m_FWMIP = string(oppoAddr.getIP());        
    m_FWMProcessID = 0;
    m_pInfoFromFWMToRC == NULL;
    m_frameworkID = 0;
    m_fwInstanceID = 0;
}

FWMAgent::~FWMAgent()
{
    if(m_pInfoFromFWMToRC != NULL)
    {
        delete m_pInfoFromFWMToRC;
        m_pInfoFromFWMToRC = NULL;
    }
}

int FWMAgent::init(void)
{
    m_pInfoFromFWMToRC = new InfoFromFWMToRC(getID(), m_FWMIP, m_frameworkID, m_fwInstanceID);
    return SUCCESSFUL;
}

void FWMAgent::readBack(InReq &req)
{
    int ret = m_pInfoFromFWMToRC->handleFWMReq(req);
    if(ret < 0)
    {
        ERROR_LOG("FWMAgent::readBack : ret < 0");
    }
}

void FWMAgent::setMsgHeader(
    uint32_t cmd, uint32_t err, uint32_t len, uint64_t tid)
{
    m_msg.cmd = cmd;
    m_msg.length = len;
    m_msg.error = err;
    m_msg.para1 = tid;
    m_msg.para2 = tid >> 32;
}


int FWMAgent::sendPackage(MsgHeader &msg, string data)
{
    uint32_t buffLen = HEADER_SIZE + msg.length;
    char *buff = new char[buffLen + 1];
    memset(buff, 0, buffLen + 1);
    memcpy(buff, &msg, HEADER_SIZE);
    if(msg.length != 0)
        memcpy(buff + HEADER_SIZE, data.c_str(), msg.length);

    if(TCPAgent::writeToBuffer(buff, buffLen) < 0)
    {
        ERROR_LOG("FWMAgent::sendPackage : writeToBuffer Error");
        if(buff != NULL)
        {
            delete [] buff;
            buff = NULL;
        }
        return FAILED;
    }

    return SUCCESSFUL;
}

void FWMAgent::recycler(void)
{
    TCPAgent::recycler();
}

}

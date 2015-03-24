#ifndef _RC_NCAGENT_H_
#define _RC_NCAGENT_H_

#include "InfoFromNCToRC.h"

#include "common/comm/TCPAgent.h"
#include "common/comm/Epoll.h"

namespace rc
{

class NCAgent : public TCPAgent
{
public:
    NCAgent(const TCPSocket &sock,
            const SocketAddress &oppoAddr);
    ~NCAgent();

    int init(void);
    void readBack(InReq&);
    void recycler(void);

    void setMsgHeader(uint32_t, uint32_t err = 0, 
            uint32_t len = 0, uint64_t tid = 0);
    int sendPackage(MsgHeader&, string data = "");

    void setIP(string ip)
    {
        m_NCIP = ip;
    }

    string getIP() const
    {
        return m_NCIP;
    }

    InfoFromNCToRC * getInfoFromNCToRC() const
    {
        return m_pInfoFromNCToRC;
    }

private:
    string m_NCIP;
    MsgHeader m_msg;
    InfoFromNCToRC *m_pInfoFromNCToRC;
};

}

#endif

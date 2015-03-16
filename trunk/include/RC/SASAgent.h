#ifndef _RC_SASAGENT_H_
#define _RC_SASAGENT_H_


#include "common/comm/TCPAgent.h"
#include "common/comm/Epoll.h"

namespace rc
{

class SASAgent : public TCPAgent
{
public:
    SASAgent(const TCPSocket &sock,
            const SocketAddress &oppoAddr);
    ~SASAgent();

    void readBack(InReq&);
    void recycler(void);

    void setMsgHeader(uint32_t, uint32_t err = 0, 
            uint32_t len = 0, uint64_t tid = 0);
    int sendPackage(MsgHeader&, string data = "");

private:
    MsgHeader m_msg;
};

}

#endif

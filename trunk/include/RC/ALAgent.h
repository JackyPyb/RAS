#ifndef _RC_ALAGENT_H_
#define _RC_ALAGENT_H_


#include "common/comm/TCPAgent.h"
#include "common/comm/Epoll.h"

namespace rc
{

class ALAgent : public TCPAgent
{
public:
    ALAgent(const TCPSocket &sock,
            const SocketAddress &oppoAddr);
    ~ALAgent();

    void readBack(InReq&);
    void recycler(void);

    void setMsgHeader(uint32_t, uint32_t err = 0, 
            uint32_t len = 0, uint64_t tid = 0);
    int sendPackage(MsgHeader&, string data = "");
    uint64_t mergeID(uint32_t low, uint32_t high) const;

private:
    MsgHeader m_msg;
};

}

#endif

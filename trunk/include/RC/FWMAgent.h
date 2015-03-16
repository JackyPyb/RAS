#ifndef _RC_FWMAGENT_H_
#define _RC_FWMAGENT_H_

#include "InfoFromFWMToRC.h"

#include "common/comm/TCPAgent.h"
#include "common/comm/Epoll.h"

namespace rc
{

class FWMAgent : public TCPAgent
{
public:
    FWMAgent(const TCPSocket &sock,
            const SocketAddress &oppoAddr);
    ~FWMAgent();

    int init(void);
    void readBack(InReq&);
    void recycler(void);

    void setMsgHeader(uint32_t, uint32_t err = 0, 
            uint32_t len = 0, uint64_t tid = 0);
    int sendPackage(MsgHeader&, string data = "");

private:
    string m_FWMIP;
    uint32_t m_FWMProcessID;
    MsgHeader m_msg;
    InfoFromFWMToRC *m_pInfoFromFWMToRC;
    uint32_t m_frameworkID;
    uint32_t m_fwInstanceID;
};

}

#endif

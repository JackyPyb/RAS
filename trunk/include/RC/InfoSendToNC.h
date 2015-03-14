#ifndef _RC_INFOSENDTONC_H_
#define _RC_INFOSENDTONC_H_

#include "common/comm/BaseHeader.h"

#include "Task.h"

namespace rc
{

class InfoSendToNC
{
public:
    InfoSendToNC(uint32_t);
    ~InfoSendToNC();

    void setMsgHeader(uint32_t, uint32_t, uint64_t);
    int sendTaskToNC(Task*);

private:
    uint32_t m_NCAgentID;
    MsgHeader m_msg;
    string m_data;

};

}

#endif

#ifndef _RC_INFOSENDTOFWM_H_
#define _RC_INFOSENDTOFWM_H_

#include "common/comm/BaseHeader.h"

#include "Task.h"

namespace rc
{

class InfoSendToFWM
{
public:
    InfoSendToFWM(uint32_t);
    ~InfoSendToFWM();

    void setMsgHeader(uint32_t, uint32_t, uint64_t);
    int sendTaskToFWM(Task*);

private:
    uint32_t m_FWMAgentID;
    MsgHeader m_msg;
    string m_data;

};

}

#endif

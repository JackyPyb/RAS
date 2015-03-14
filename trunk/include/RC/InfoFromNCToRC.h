#ifndef _RC_INFOFROMNCTORC_H_
#define _RC_INFOFROMNCTORC_H_

#include "common/comm/BaseReq.h"

#include <stdint.h>
#include <string>

using std::string;

namespace rc
{

class InfoFromNCToRC
{
public:
    InfoFromNCToRC(uint32_t, string);
    ~InfoFromNCToRC();

    int handleNCReq(InReq &);
    int taskReportFromNC(uint32_t, uint64_t);
    int doRegister(uint64_t, string);
    void sendAckToNC(uint32_t, uint32_t, uint32_t, uint32_t);
    uint64_t mergeID(uint32_t, uint32_t) const;

private:
    uint32_t m_NCAgentID;
    string m_NCIP;

};

}

#endif

#ifndef _RC_INFOFROMFWMTORC_H_
#define _RC_INFOFROMFWMTORC_H_

#include "common/comm/BaseReq.h"

#include <stdint.h>
#include <string>

using std::string;

namespace rc
{

class InfoFromFWMToRC
{
public:
    InfoFromFWMToRC(uint32_t, string, uint32_t, uint32_t);
    ~InfoFromFWMToRC();

    int handleFWMReq(InReq &);
    int taskReportFromFWM(uint32_t, uint64_t);
    int doRegister(uint64_t, string);
    void sendAckToFWM(uint32_t, uint32_t, uint32_t, uint32_t);
    uint64_t mergeID(uint32_t, uint32_t) const;

private:
    uint32_t m_FWMAgentID;
    string m_FWMIP;
    uint32_t m_frameworkID;
    uint32_t m_fwInstanceID;

};

}

#endif

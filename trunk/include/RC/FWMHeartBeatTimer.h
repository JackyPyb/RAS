#ifndef _RC_FWMHEARTBEATTIMER_H_
#define _RC_FWMHEARTBEATTIMER_H_

#include "common/Timer/Timer.h"

#include <string>

using std::string;

namespace rc
{

class FWMHeartBeatTimer : public Timer
{
public:
    FWMHeartBeatTimer(unsigned int, uint32_t, string, uint32_t, uint32_t);
    ~FWMHeartBeatTimer();

    int doAction();
    void setRetryNum(unsigned int);
    unsigned int getRetryNum() const;
    void resetRetryNum();

private:
    unsigned int m_retryNum;
    uint32_t m_FWMAgentID;
    string m_FWMIP;
    uint32_t m_frameworkID;
    uint32_t m_fwInstanceID;
};    

}

#endif

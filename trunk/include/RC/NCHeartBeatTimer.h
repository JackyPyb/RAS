#ifndef _RC_NCHEARTBEATTIMER_H_
#define _RC_NCHEARTBEATTIMER_H_

#include "common/Timer/Timer.h"

#include <string>

using std::string;

namespace rc
{

class NCHeartBeatTimer : public Timer
{
public:
    NCHeartBeatTimer(unsigned int, uint32_t, string);
    ~NCHeartBeatTimer();

    int doAction();
    void setRetryNum(unsigned int);
    unsigned int getRetryNum() const;
    void resetRetryNum();

private:
    unsigned int m_retryNum;
    uint32_t m_NCAgentID;
    string m_NCIP;
};    

}

#endif

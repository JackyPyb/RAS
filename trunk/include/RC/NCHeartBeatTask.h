#ifndef _RC_NCHEARTBEATTASK_H_
#define _RC_NCHEARTBEATTASK_H_

#include "ContainResourceTask.h"

namespace rc
{

const uint32_t NCHEARTBEATTASK_UPDATE_NCINFO = 1;
const uint32_t NCHEARTBEATTASK_UPDATE_FWIINFO = 2;
const uint32_t NCHEARTBEATTASK_FINISH_TASK = 3;

class NCHeartBeatTask : public ContainResourceTask
{
public:
    NCHeartBeatTask();
    ~NCHeartBeatTask();

    int goNext();
    int updateNCInfo();
    int updateFWIInfo();
    void clearTaskPara();

    void setNCIP(const string &ip)
    {
        m_NCIP = ip;
    }

    string getNCIP() const
    {
        return m_NCIP;
    }

private:
    string m_NCIP;

};

}

#endif

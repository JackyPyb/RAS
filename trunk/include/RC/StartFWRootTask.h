#ifndef _RC_STARTFWROOTTASK_H_
#define _RC_STARTFWROOTTASK_H_

#include "ContainResourceTask.h"
#include "protocol/AlProtocol.pb.h"

#include <string>

using std::string;

namespace rc
{

const uint32_t STARTFWROOTTASK_DOPARSE = 1;
const uint32_t STARTFWROOTTASK_SET_TASK_SCHEDULER_TYPE = 2;
const uint32_t STARTFWROOTTASK_CREATE_FWINSTANCE = 3;
const uint32_t STARTFWROOTTASK_SET_FWINSTANCE_BASIC_INFO = 4;
const uint32_t STARTFWROOTTASK_SCHEDULER = 5;
const uint32_t STARTFWROOTTASK_WAIT_NC_ACK = 6;
const uint32_t STARTFWROOTTASK_FINISH_TASK = 7;

class StartFWRootTask : public ContainResourceTask
{
public:
    StartFWRootTask();
    ~StartFWRootTask();

    int goNext();
    int doParse();
    int setTaskSchedulerType();
    int sendResultToAL(const uint32_t);

    void setAgentID(uint32_t aid)
    {
        m_ALAgentID = aid;
    }

    uint32_t getAgentID() const
    {
        return m_ALAgentID;
    }

    void setRestart()
    {
        m_isRestart = true;
    }

    void setFirstTimeStart()
    {
        m_isRestart = false;
    }

    bool isRestart() const
    {
        return m_isRestart;
    }

    void setRootModuleID(uint32_t id)
    {
        m_rootModuleID = id;
    }

    uint32_t getRootModuleID() const
    {
        return m_rootModuleID;
    }

    void setNCIP(const string &ip)
    {
        m_NCIP= ip;
    }


    void clearTaskPara();

private:    
    uint32_t m_ALAgentID;
    bool m_isRestart;
    uint32_t m_rootModuleID;
    string m_NCIP;
    uint32_t m_fwInstanceID;
    AlProto::StartRootModule m_startRootModuleInfo; 
};

}

#endif

#ifndef _RC_TASK_H_
#define _RC_TASK_H_

#include "common/comm/BaseTask.h"

#include <stdint.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace rc
{

class Task : public BaseTask
{

public:
    Task(){}
    virtual ~Task(){}
    
    void setTaskType(const uint32_t taskType)
    {
        m_taskType = taskType;
    }

    uint32_t getTaskType() const
    {
        return m_taskType;
    }

    void setSchedulerType(const uint32_t schedulerType)
    {
        m_schedulerType = schedulerType;
    }

    uint32_t getSchedulerType() const
    {
        return m_schedulerType;
    }

    virtual void setNCIP(const string&){}
    virtual string getNCIP() const{}
    virtual int goNext()
    {
        return 0;
    }

    void setTaskErr(const uint32_t error)
    {
        m_error = error;
    }

    uint32_t nextTaskState()
    {
        m_taskState++;
        return goNext();
    }

    void setTaskState(const uint32_t  state)
    {
        m_taskState = state;
    }

    uint32_t getTaskState() const
    {
        return m_taskState;
    }

    void setDataString(const string &data)
    {
        m_data = data;
    }

    string getDataString() const
    {
        return m_data;
    }

    virtual void clearTaskPara() {}

private:
    uint32_t m_taskType;
    uint32_t m_schedulerType;
    uint32_t m_taskState;
    string m_data;
    uint32_t m_error;

};

}

#endif

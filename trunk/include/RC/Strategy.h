#ifndef _RC_STRATEGY_H_
#define _RC_STRATEGY_H_

#include "Task.h"

namespace rc
{

class Strategy
{

public:
    virtual ~Strategy(){}
    virtual int dispatch(Task*)
    {
        return 0;
    }

    uint32_t getStrategyID()
    {
        return m_sID;
    }

    void setStrategyID(uint32_t id)
    {
        m_sID = id;
    }

private:
    uint32_t m_sID;

};

}

#endif

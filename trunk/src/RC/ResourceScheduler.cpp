#include "ResourceScheduler.h"
#include "StrategyManager.h"

#include "common/comm/Error.h"'
#include "common/log/log.h"

namespace rc
{

ResourceScheduler::ResourceScheduler()    
{

}

ResourceScheduler::~ResourceScheduler()
{

}

int ResourceScheduler::scheduleTask(uint64_t tid)
{
    return SUCCESSFUL;
}

int ResourceScheduler::NCCallBack(string ip, uint64_t tid, uint32_t err)
{
    return SUCCESSFUL;
}

}

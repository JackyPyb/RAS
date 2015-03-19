#include "ResourceScheduler.h"
#include "StrategyManager.h"

#include "common/comm/TaskManager.h"

#include "common/comm/Error.h"
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
    Task *pTask = dynamic_cast<Task*>(
            (TaskManager::getInstance())->get(tid));
    if(pTask == NULL)
    {
        INFO_LOG(
                "ResourceScheduler::scheduleTask: no this task");
        return FAILED;
    }

    uint32_t scheduleType = pTask->getSchedulerType();
    Strategy *pStrategy = 
        (StrategyManager::getInstance())->getStrategy(scheduleType);
    if(pStrategy == NULL)
    {
        INFO_LOG(
                "ResourceScheduler::shcedulerTask: no this strategy");
        return FAILED;
    }
    pStrategy->dispatch(pTask);

    return SUCCESSFUL;
}

int ResourceScheduler::NCCallBack(string ip, uint64_t tid, uint32_t err)
{
    return SUCCESSFUL;
}

}

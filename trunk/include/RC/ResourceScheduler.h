#ifndef _RC_RESOURCESCHEDULER_H_
#define _RC_RESOURCESCHEDULER_H_

#include "Task.h"

#include "common/sys/Singleton.h"

namespace rc
{

class ResourceScheduler : public Singleton<ResourceScheduler>
{
protected:
    ResourceScheduler();
    ~ResourceScheduler();

public:
    friend class Singleton<ResourceScheduler>;

    int scheduleTask(uint64_t);
    int NCCallBack(string, uint64_t, uint32_t);
};    

}

#endif

#include "GetNCFromAll.h"
#include "ResourceManager.h"
#include "ContainResourceTask.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

int GetNCFromAll::dispatch(Task *pTask)
{
    ContainResourceTask * pResTask = dynamic_cast<ContainResourceTask*>(pTask);
    if(pResTask != NULL)
    {    
        Resource platformRes = (ResourceManager::getInstance())
            ->getPlatformTotalRes();
        Resource taskRes = ((pResTask->getResourceMap()).find("*"))->second;

        bool mainByMem = true;
        double memRatio = 
            ((double)taskRes.cpuMemSize) / ((double)platformRes.cpuMemSize);
        double cpuRatio = 
            ((double)taskRes.logicCPUNum) / ((double)taskRes.logicCPUNum);
        if(memRatio < cpuRatio)
        {
            mainByMem = false;
        }

        string ncIP = "";
        if(mainByMem)
        {
            ncIP = (ResourceManager::getInstance())
                ->getSuitableNCByMem(taskRes);
        }
        else
        {
            ncIP = (ResourceManager::getInstance())
                ->getSuitableNCByCPU(taskRes);
        }

        if(ncIP.compare("") == 0)
        {
            return FAILED;
        }
        else
        {

        }
    }
    else
    {

    }

    return 0;
}

}

#include "GetNCFromAll.h"
#include "ResourceManager.h"
#include "ContainResourceTask.h"
#include "NCLoadBalance.h"
#include "StartFWRootTask.h"
#include "FWIManager.h"
#include "FWInstance.h"
#include "protocol/TaskType.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

int GetNCFromAll::dispatch(Task *pTask)
{
    ContainResourceTask * pResTask = dynamic_cast<ContainResourceTask*>(pTask);
    string ncIP = "";
    NCLoadBalance *pNCLB = NULL;
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
            pNCLB = 
                (ResourceManager::getInstance())->getNCLB(ncIP);
            if(pResTask->getTaskType() == START_FW_ROOT_TASK)
            {
                StartFWRootTask *pStartRootTask = 
                    dynamic_cast<StartFWRootTask*>(pResTask);
                if(pStartRootTask == NULL)
                {
                    ERROR_LOG(
                            "GetNCFromAll::dispatch: dynamic_cast \
                            START_FW_ROOT_TASK error!");
                    return FAILED;
                }
                uint32_t fwInstanceID = pStartRootTask->getFWInstanceID();
                pNCLB->addFWInstance(fwInstanceID);
                pNCLB->addApplyRes(taskRes);
                (ResourceManager::getInstance())->deleteNCLBInSets(ncIP);
                (ResourceManager::getInstance())->addNCLBToSets(ncIP);
                FWInstance *pFWInstance = 
                    (FWIManager::getInstance())->get(fwInstanceID);
                pFWInstance->addNCApplyRes(ncIP, taskRes);
                uint32_t moduleID = 
                    (FWIManager::getInstance())->generateModuleID();
                pFWInstance->addRootModule(moduleID);
                pStartRootTask->setRootModuleID(moduleID);
            }
        }

        pTask->setNCIP(ncIP);
        pNCLB->sendTaskToNC(pTask);
    }
    else
    {

    }

    return SUCCESSFUL;
}

}

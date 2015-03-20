#include "GetNCFromOne.h"
#include "ContainResourceTask.h"
#include "ResourceManager.h"
#include "NCLoadBalance.h"
#include "FWInstance.h"
#include "protocol/TaskType.h"
#include "StartFWRootTask.h"
#include "FWIManager.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

int GetNCFromOne::dispatch(Task *pTask)
{
    string ncIP = pTask->getNCIP();
    NCLoadBalance *pNCLB = (ResourceManager::getInstance())->getNCLB(ncIP);

    ContainResourceTask *pResTask = dynamic_cast<ContainResourceTask*>(pTask);
    if(pResTask != NULL)
    {
         Resource taskRes = ((pResTask->getResourceMap()).find("*"))->second;
         Resource ncRes = pNCLB->getNotApplyRes();
         if((ncRes.cpuMemSize >= taskRes.cpuMemSize) &&
                 ncRes.logicCPUNum >= taskRes.logicCPUNum)
         {
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
         else
         {
            return FAILED;
         }
    }
    else
    {

    }

    pNCLB->sendTaskToNC(pTask);
    return SUCCESSFUL;
}

}

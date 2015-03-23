#include "GetNCFromOne.h"
#include "ContainResourceTask.h"
#include "ResourceManager.h"
#include "NCLoadBalance.h"
#include "FWInstance.h"
#include "protocol/TaskType.h"
#include "StartFWRootTask.h"
#include "FWIManager.h"
#include "head.h"

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
         Resource taskRes = ((pResTask->getResourceMap()).find(ncIP))->second;
         Resource ncRes = pNCLB->getNotApplyRes();
#ifdef DEBUG
         INFO_LOG("GetNCFromOne::dispatch");
         INFO_LOG("IP %s resource: cpu %f, mem %d", ncIP.c_str(), 
                 ncRes.logicCPUNum, ncRes.cpuMemSize);
         INFO_LOG("Task resource: cpu %f, mem %d", taskRes.logicCPUNum, taskRes.cpuMemSize);
#endif
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
#ifdef DEBUG
                 INFO_LOG("-------After dispatch-------");
                 INFO_LOG("IP %s, apply resource is \n \
                         logicCPUNum is %f, cpuMemSize is %d",
                         ncIP.c_str(),
                         pNCLB->getApplyRes().logicCPUNum,
                         pNCLB->getApplyRes().cpuMemSize);

                 INFO_LOG("****** Mem set");
                 (ResourceManager::getInstance())->printMemSet();
                 INFO_LOG("****** CPU set");
                 (ResourceManager::getInstance())->printCPUSet();

                 Resource debugRes = pFWInstance->getNCApplyRes(ncIP);
                 INFO_LOG("FW Instance ID %d in NC %s total apply res is \n \
                         logicCPUNum is %f, cpuMemSize is %d",
                         fwInstanceID,
                         ncIP.c_str(),
                         debugRes.logicCPUNum,
                         debugRes.cpuMemSize);
                 INFO_LOG("Root module ID is %d",
                         pStartRootTask->getRootModuleID());
#endif
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

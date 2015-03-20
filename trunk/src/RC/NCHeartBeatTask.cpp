#include "NCHeartBeatTask.h"
#include "ResourceManager.h"
#include "NCLoadBalance.h"
#include "Resource.h"
#include "protocol/RcNcProtocol.pb.h"
#include "protocol/TaskType.h"
#include "FWInstance.h"
#include "FWIManager.h"

#include "common/comm/TaskManager.h"
#include "common/log/log.h"
#include "common/comm/Error.h"

namespace rc
{

NCHeartBeatTask::NCHeartBeatTask():
    m_NCIP("")
{
    setTaskState(NCHEARTBEATTASK_UPDATE_NCINFO);
    setDataString("");
    setTaskType(NC_HEART_BEAT_TASK);
}

NCHeartBeatTask::~NCHeartBeatTask()
{

}

int NCHeartBeatTask::goNext()
{
    int ret = SUCCESSFUL;
    switch(getTaskState())
    {
        case NCHEARTBEATTASK_UPDATE_NCINFO:
        {
            ret = updateNCInfo();
            setTaskState(NCHEARTBEATTASK_UPDATE_FWIINFO);
            goNext();
            break;
        }
        case NCHEARTBEATTASK_UPDATE_FWIINFO:
        {
            ret = updateFWIInfo();
            setTaskState(NCHEARTBEATTASK_FINISH_TASK);
            goNext();
            break;
        }
        case NCHEARTBEATTASK_FINISH_TASK:
        {
            INFO_LOG("Update NC info OK!");
            clearTaskPara();
            (TaskManager::getInstance())->recycle(getID());
            break;
        }
        default:
            ret = FAILED;
            break;
    }

    return ret;
}

int NCHeartBeatTask::updateNCInfo()
{
    NCLoadBalance *pNCLB= (ResourceManager::getInstance())->getNCLB(m_NCIP);
    if(pNCLB == NULL)
    {
        ERROR_LOG(
                "NCHeartBeatTask::updateNCInfo: NCLoadBalance not found");
        return FAILED;
    }

    Resource actualUseRes;
    Resource actualRemainRes;

    RcNcProto::MonitorInfo monitorInfo;
    if(!monitorInfo.ParseFromString(getDataString()))
    {
        ERROR_LOG(
                "NCHeartBeatTask::updateNCInfo: parse protobuffer error!");
        return FAILED;
    }

    RcNcProto::MachineResourceInfo usedResInfo = 
        monitorInfo.usage_machine_resource_info();
    actualUseRes.logicCPUNum = usedResInfo.cpu_num();
    actualUseRes.cpuMemSize = usedResInfo.cpu_mem_size();

    for(int i = 0; i < usedResInfo.gpu_resource_info_size(); i++)
    {
        RcNcProto::GpuResourceInfo usedGPUInfo = 
             usedResInfo.gpu_resource_info(i);
        string gpuName = usedGPUInfo.gpu_name();
        actualUseRes.GPUInfo[gpuName] = usedGPUInfo.gpu_mem_size();
    }
    
    pNCLB->setActualUseRes(actualUseRes);

    RcNcProto::ResourceInfo remainResInfo = 
        monitorInfo.rest_machine_resource_info();
    actualRemainRes.logicCPUNum = remainResInfo.cpu_num();
    actualRemainRes.cpuMemSize = remainResInfo.cpu_mem_size();

    for(int i = 0; remainResInfo.gpu_resource_info_size(); i++)
    {
        RcNcProto::GpuResourceInfo remainGPUInfo = 
            remainResInfo.gpu_resource_info(i);
        string gpuName = remainGPUInfo.gpu_name();
        actualRemainRes.GPUInfo[gpuName] = remainGPUInfo.gpu_mem_size();
    }

    pNCLB->setActualRemainRes(actualRemainRes);

    return SUCCESSFUL;
}

int NCHeartBeatTask::updateFWIInfo()
{

    RcNcProto::MonitorInfo monitorInfo;
    if(!monitorInfo.ParseFromString(getDataString()))
    {
        ERROR_LOG(
                "NCHeartBeatTask::updateFWIInfo: parse protobuffer error!");
        return FAILED;
    }

    for(int i = 0; i < monitorInfo.container_usage_resource_info_size(); i++)
    {
        RcNcProto::ContainerResourceInfo usedFWResInfo =
            monitorInfo.container_usage_resource_info(i);
        uint32_t fwID = usedFWResInfo.framework_instance_id();

        Resource res;
        RcNcProto::ResourceInfo resInfo = usedFWResInfo.resource_info();
        res.logicCPUNum = resInfo.cpu_num();
        res.cpuMemSize = resInfo.cpu_mem_size();
        for(int j = 0; j < resInfo.gpu_resource_info_size(); j++)
        {
            RcNcProto::GpuResourceInfo fwGPUInfo = 
                resInfo.gpu_resource_info(j);
            string name = fwGPUInfo.gpu_name();
            res.GPUInfo[name] = fwGPUInfo.gpu_mem_size();
        }

        FWInstance *pFWInstance = (FWIManager::getInstance())->get(fwID);
        if(pFWInstance == NULL)
        {
            ERROR_LOG(
                    "NCHeartBeatTask::updateFWIInfo: FWInstance not found");
            return FAILED;
        }

        Resource oldRes = pFWInstance->getNCActualUseRes(m_NCIP);
        pFWInstance->setNCActualUseRes(m_NCIP, res);
        Resource totalUsedRes = pFWInstance->getTotalActualUseRes();
        totalUsedRes -= oldRes;
        totalUsedRes += res;
        pFWInstance->setTotalActualUseRes(totalUsedRes);
    }

    return SUCCESSFUL;
}

void NCHeartBeatTask::clearTaskPara()
{
    m_NCIP.clear();
}

}

#include "NCHeartBeatTask.h"
#include "ResourceManager.h"
#include "NCLoadBalance.h"
#include "Resource.h"
#include "protocol/RcNcProtocol.pb.h"
#include "protocol/TaskType.h"
#include "FWInstance.h"
#include "FWIManager.h"
#include "head.h"

#include "common/comm/TaskManager.h"
#include "common/log/log.h"
#include "common/comm/Error.h"

namespace rc
{

NCHeartBeatTask::NCHeartBeatTask():
    m_NCIP("")
{
#ifdef DEBUG
    INFO_LOG("NCHeartBeatTask::NCHeartBeatTask");
#endif
    setTaskState(NCHEARTBEATTASK_UPDATE_NCINFO);
    setDataString("");
    setTaskType(NC_HEART_BEAT_TASK);
}

NCHeartBeatTask::~NCHeartBeatTask()
{
    m_NCIP.clear();
}

int NCHeartBeatTask::goNext()
{
    int ret = SUCCESSFUL;
    switch(getTaskState())
    {
        case NCHEARTBEATTASK_UPDATE_NCINFO:
        {
            #ifdef DEBUG
            INFO_LOG("NCHEARTBEATTASK_UPDATE_NCINFO");
            #endif

            ret = updateNCInfo();
            setTaskState(NCHEARTBEATTASK_UPDATE_FWIINFO);
            goNext();
            break;
        }
        case NCHEARTBEATTASK_UPDATE_FWIINFO:
        {
            #ifdef DEBUG
            INFO_LOG("NCHEARTBEATTASK_UPDATE_FWIINFO");
            #endif

            ret = updateFWIInfo();
            setTaskState(NCHEARTBEATTASK_FINISH_TASK);
            goNext();
            break;
        }
        case NCHEARTBEATTASK_FINISH_TASK:
        {
            #ifdef DEBUG
            INFO_LOG("Update NC info OK!");
            #endif

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
#ifdef DEBUG
    INFO_LOG("parse::: usedResInfo.cpu_num is %f, mem is %d", usedResInfo.cpu_num(), usedResInfo.cpu_mem_size());
#endif

    for(int i = 0; i < usedResInfo.gpu_resource_info_size(); i++)
    {
        RcNcProto::GpuResourceInfo usedGPUInfo = 
             usedResInfo.gpu_resource_info(i);
        string gpuName = usedGPUInfo.gpu_name();
        actualUseRes.GPUInfo[gpuName] = usedGPUInfo.gpu_mem_size();
    }
    
    pNCLB->setActualUseRes(actualUseRes);
    
    #ifdef DEBUG
    Resource debugUsedResInfo = pNCLB->getActualUseRes();
    INFO_LOG("NC %s actual use res is \n \
            logicCPUNum is %f, cpuMemSize is %d", 
            getNCIP().c_str(),
            debugUsedResInfo.logicCPUNum,
            debugUsedResInfo.cpuMemSize);
    #endif

    RcNcProto::ResourceInfo remainResInfo = 
        monitorInfo.rest_machine_resource_info();
    actualRemainRes.logicCPUNum = remainResInfo.cpu_num();
    actualRemainRes.cpuMemSize = remainResInfo.cpu_mem_size();

    for(int i = 0; i < remainResInfo.gpu_resource_info_size(); i++)
    {
        RcNcProto::GpuResourceInfo remainGPUInfo = 
            remainResInfo.gpu_resource_info(i);
        string gpuName = remainGPUInfo.gpu_name();
        actualRemainRes.GPUInfo[gpuName] = remainGPUInfo.gpu_mem_size();
    }

    pNCLB->setActualRemainRes(actualRemainRes);

    #ifdef DEBUG
    Resource debugRemainRes = pNCLB->getActualRemainRes();
    INFO_LOG("NC %s actual remain res is \n \
            logicCPUNum is %f, cpuMemSize is %d",
            m_NCIP.c_str(),
            debugRemainRes.logicCPUNum,
            debugRemainRes.cpuMemSize);
    #endif

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

    #ifdef DEBUG
    INFO_LOG("NC %s fw num is %d",
            m_NCIP.c_str(),
            monitorInfo.container_usage_resource_info_size());
    #endif

    for(int i = 0; i < monitorInfo.container_usage_resource_info_size(); i++)
    {
        RcNcProto::ContainerResourceInfo usedFWResInfo =
            monitorInfo.container_usage_resource_info(i);
        uint32_t fwInstanceID = usedFWResInfo.framework_instance_id();

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

        FWInstance *pFWInstance = (FWIManager::getInstance())->get(fwInstanceID);
        if(pFWInstance == NULL)
        {
            ERROR_LOG(
                    "NCHeartBeatTask::updateFWIInfo: FWInstance not found");
            return FAILED;
        }
        
        Resource oldRes = pFWInstance->getNCActualUseRes(m_NCIP);
        pFWInstance->setNCActualUseRes(m_NCIP, res);
        Resource totalUsedRes = pFWInstance->getTotalActualUseRes();

        #ifdef DEBUG
        INFO_LOG("NC %s, FW Instance ID %d, actual use res is \n \
                logicCPUNum is %f, cpuMemSize is %d",
                m_NCIP.c_str(),
                fwInstanceID,
                res.logicCPUNum,
                res.cpuMemSize);
        INFO_LOG("NC %s , FW Instance ID %d, old actual use res is \n \
                logicCPUNum is %f, cpuMemSize is %d",
                m_NCIP.c_str(),
                fwInstanceID,
                oldRes.logicCPUNum,
                oldRes.cpuMemSize);
        INFO_LOG("FW Instance ID %d ---OLD--- total actual res is \n \
                logicCPUNum is %f, cpuMemSize is %d",
                fwInstanceID,
                totalUsedRes.logicCPUNum,
                totalUsedRes.cpuMemSize);
        #endif

        totalUsedRes -= oldRes;
        totalUsedRes += res;
        pFWInstance->setTotalActualUseRes(totalUsedRes);

        #ifdef DEBUG
        Resource newTotalUsedRes = pFWInstance->getTotalActualUseRes();
        INFO_LOG("FW Instance ID %d ---NEW--- total actual res is \n \
                logicCPUNum is %f, cpuMemSize is %d",
                fwInstanceID,
                newTotalUsedRes.logicCPUNum,
                newTotalUsedRes.cpuMemSize);
        #endif
    }

    return SUCCESSFUL;
}

void NCHeartBeatTask::clearTaskPara()
{
    m_NCIP.clear();
}

}

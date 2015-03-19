#include "StartFWRootTask.h"
#include "protocol/SchedulerType.h"
#include "FWIManager.h"
#include "FWInstance.h"
#include "ResourceScheduler.h"

#include "common/log/log.h"
#include "common/comm/Error.h"

namespace rc
{
 
StartFWRootTask::StartFWRootTask():
    m_isRestart(false),
    m_rootModuleID(0),
    m_NCIP(""),
    m_fwInstanceID(0)
{
    setTaskState(STARTFWROOTTASK_DOPARSE);
    setDataString("");
}

StartFWRootTask::~StartFWRootTask()
{
    m_NCIP.clear();
}

int StartFWRootTask::goNext()
{
    int ret = SUCCESSFUL;
    switch(getTaskState())
    {
        case STARTFWROOTTASK_DOPARSE:
        {
            doParse();
            goNext();
            break;
        }
        case STARTFWROOTTASK_SET_TASK_SCHEDULER_TYPE:
        {
            setTaskSchedulerType();
            goNext();
            break;
        }
        case STARTFWROOTTASK_CREATE_FWINSTANCE:
        {
            FWInstance *pFWInstance = (FWIManager::getInstance())->createFWInstance();
            m_fwInstanceID = pFWInstance->getFWInstanceID();
            goNext();
            break;
        }
        case STARTFWROOTTASK_SET_FWINSTANCE_BASIC_INFO:
        {
            FWInstance *pFWInstance = (FWIManager::getInstance())->get(m_fwInstanceID);
            uint32_t frameworkID = m_startRootModuleInfo.framework_id();
            pFWInstance->setFrameworkID(frameworkID);
            AlProto::ResourceInfo rootRes = m_startRootModuleInfo.request_resource_size();
            double cpuNum = rootRes.cpu_num();
            uint32_t memSize = rootRes.cpu_mem_size();
            pFWInstance->setRootLogicCPUNum(cpuNum);
            pFWInstance->setRootMemSize(memSize);

            multimap<string, uint32_t> gpuMap;
            for(int i = 0; i < rootRes.gpu_resource_info_size(); i++)
            {
                AlProto::GpuResourceInfo gpuRes = 
                    rootRes.gpu_resource_info(i);
                string name = gpuRes.gpu_name();
                gpuMap.insert(std::pair<string, uint32_t>(name, gpuRes.gpu_mem_size()));
            }
            pFWInstance->setRootGPUInfo(gpuMap);

            multimap<string, Resource> resMap;
            Resource res;
            res.logicCPUNum = cpuNum;
            res.cpuMemSize = memSize;
           // res.GPUInfo = gpuMap;

            string ip = m_startRootModuleInfo.nc_ip();
            resMap.insert(std::pair<string, Resource>(ip, res));
            setResourceMap(resMap);
            goNext();
            break;
        }
        case STARTFWROOTTASK_SCHEDULER:
        {
            (ResourceScheduler::getInstance())->scheduleTask(getID());
            setTaskState(STARTFWROOTTASK_WAIT_NC_ACK);
            goNext();
            break;
        }
        case STARTFWROOTTASK_WAIT_NC_ACK:
        {

        }
        default:
            ret = FAILED;
            break;
    }

    return ret;
}

int StartFWRootTask::doParse()
{
    if(!m_startRootModuleInfo.ParseFromString(getDataString()))
    {
        ERROR_LOG(
                "StartFWRootTask::doParse: parse protobuffer error");
        return FAILED;
    }

    return SUCCESSFUL;
}

int StartFWRootTask::setTaskSchedulerType()
{
    string ncIP = m_startRootModuleInfo.nc_ip();
    
    if(ncIP.compare("*") == 0)
    {
        setSchedulerType(GETNCFROMALL);
    }
    else if(ncIP.find(',') == string::npos)
    {
        setSchedulerType(GETNCFROMONE);
        setNCIP(ncIP);
    }
    else
    {

    }

    return SUCCESSFUL;    
}

int StartFWRootTask::sendResultToAL(uint32_t ret)
{
    return SUCCESSFUL;
}

void StartFWRootTask::clearTaskPara()
{
    m_NCIP.clear();
}

}

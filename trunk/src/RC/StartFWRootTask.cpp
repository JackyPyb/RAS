#include "StartFWRootTask.h"
#include "protocol/SchedulerType.h"
#include "FWIManager.h"
#include "FWInstance.h"
#include "ResourceScheduler.h"
#include "protocol/TaskType.h"
#include "ALAgent.h"
#include "protocol/RcNcProtocol.pb.h"
#include "protocol/RASCmdCode.h"

#include "common/log/log.h"
#include "common/comm/Error.h"
#include "common/comm/TaskManager.h"
#include "common/comm/AgentManager.h"

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
    setTaskType(START_FW_ROOT_TASK);
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
                gpuMap.insert(std::pair<string, uint32_t>(
                            name, gpuRes.gpu_mem_size()));
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
            break;
        }
        case STARTFWROOTTASK_WAIT_NC_ACK:
        {
            RcNcProto::RespondStartFrameworkRoot startRootAck;
            startRootAck.ParseFromString(getDataString());
            uint32_t processID = startRootAck.root_pid();
            FWInstance *pFWInstance = 
                (FWIManager::getInstance())->get(getFWInstanceID());
            pFWInstance->setModuleIPProcess(
                    getRootModuleID(),
                    getNCIP(),
                    processID);

            sendResultToAL(SUCCESSFUL);
            setTaskState(STARTFWROOTTASK_FINISH_TASK);
            goNext();
            break;
        }
        case STARTFWROOTTASK_FINISH_TASK:
        {
            INFO_LOG("Start root finish!");
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
    MsgHeader msg;
    msg.cmd = MSG_AL_RC_START_ROOT_MODULE_ACK;
    string data = "";
    if(ret >= 0)
    {
        AlProto::FrameworkInstanceInfo fwInstance;
        FWInstance *pFWInstance = 
            (FWIManager::getInstance())->get(getFWInstanceID());
        fwInstance.set_framework_id(pFWInstance->getFrameworkID());
        fwInstance.set_framework_instance_id(getFWInstanceID());
        fwInstance.SerializeToString(&data);
        msg.length = data.length();
        msg.error = SUCCESSFUL;
    }
    else
    {
        msg.length = 0;
        msg.error = FAILED;
    }

    msg.para1 = m_alTaskID;
    msg.para2 = m_alTaskID >> 32;

    ALAgent *pAgent = dynamic_cast<ALAgent*>(
            (AgentManager::getInstance())->get(getAgentID()));
    if(pAgent == NULL)
    {
        ERROR_LOG("StartFWRootTask::sendResultToAL: not found agent");
        return FAILED;
    }

    return pAgent->sendPackage(msg, data);
}

void StartFWRootTask::clearTaskPara()
{
    m_NCIP.clear();
}

}

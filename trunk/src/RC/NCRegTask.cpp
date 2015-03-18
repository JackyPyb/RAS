#include "NCRegTask.h"
#include "protocol/RcNcProtocol.pb.h"
#include "ResourceManager.h"
#include "ALAgent.h"
#include "FWMAgent.h"
#include "NCAgent.h"
#include "ConfigManager.h"
#include "protocol/RASCmdCode.h"

#include "common/log/log.h"
#include "common/comm/TaskManager.h"
#include "common/comm/TCPListenAgent.h"
#include "common/comm/AgentManager.h"

namespace rc
{

NCRegTask::NCRegTask()
{
    setTaskState(NCREGTASK_DOPARSE);
    setDataString("");
}

NCRegTask::~NCRegTask()
{

}

int NCRegTask::goNext()
{
    int ret = 0;
    switch(getTaskState())
    {
        case NCREGTASK_DOPARSE:
        {
            ret = doParse();
            setTaskState(NCREGTASK_REGISTER);
            ret = goNext();
            break;
        }
        case NCREGTASK_REGISTER:
        {
            ret = (ResourceManager::getInstance())->registerNC(
                    m_NCIP, m_NCAgentID, getID());
            sendAckToNC(ret);
            if(ret < 0)
            {

            }
            else
            {
                setTaskState(NCREGTASK_CHECK_SERVICE);
                INFO_LOG("Resgister %s OK!", m_NCIP.c_str());
                goNext();
            }
            break;
        }
        case NCREGTASK_CHECK_SERVICE:
        {
            if((ResourceManager::getInstance())->checkServiceIsOK())
            {
                setTaskState(NCREGTASK_CREATE_ALFWMLISTEN);
                goNext();
            }
            else
            {
                setTaskState(NCREGTASK_FINISH_TASK);
                goNext();
            }
            break;
        }
        case NCREGTASK_CREATE_ALFWMLISTEN:
        {
            if((ResourceManager::getInstance())->isALFWMListenCreated())
            {
                setTaskState(NCREGTASK_FINISH_TASK);
                goNext();
            }
            else
            {
                SocketAddress ALListenAddress = 
                    (ConfigManager::getInstance())->getALListenAddr();
                TCPListenAgent<ALAgent> *pALListenAgent = 
                    (AgentManager::getInstance())
                    ->createAgent< TCPListenAgent<ALAgent> >(ALListenAddress);
                pALListenAgent->init();
                INFO_LOG("Listen to AL");

                SocketAddress FWMListenAddress = 
                    (ConfigManager::getInstance())->getFWMListenAddr();
                TCPListenAgent<FWMAgent> *pFWMListenAgent = 
                    (AgentManager::getInstance())
                    ->createAgent< TCPListenAgent<FWMAgent> >(FWMListenAddress);
                pFWMListenAgent->init();
                INFO_LOG("Listen to FWM");

                (ResourceManager::getInstance())->setALFWMListenCreated(true);
                setTaskState(NCREGTASK_FINISH_TASK);
                goNext();
            }
            break;
        }
        case NCREGTASK_FINISH_TASK:
        {
            INFO_LOG("NCRegTask finished!");
            clearTaskPara();
            (TaskManager::getInstance())->recycle(getID());
            break;
        }
        default:
            ERROR_LOG("NCRegTask unknown state!");
            clearTaskPara();
            (TaskManager::getInstance())->recycle(getID());
            break;
    }

    return ret;
}

int NCRegTask::doParse()
{
    RcNcProto::Register ncReg;
    bool ret = ncReg.ParseFromString(getDataString());
    if(ret)
    {
        setNCIP(ncReg.nc_ip());
        setPort(ncReg.nc_port());

        Resource res;

        RcNcProto::ResourceInfo resInfo = ncReg.machine_total_resource();
        res.logicCPUNum = resInfo.cpu_num();
        res.cpuMemSize = resInfo.cpu_mem_size();

        for(int j = 0; j < resInfo.gpu_resource_info_size(); j++)
        {
            RcNcProto::GpuResourceInfo gpuResInfo = resInfo.gpu_resource_info(j);
            string gpuName = gpuResInfo.gpu_name();
            uint32_t gpuMemSize = gpuResInfo.gpu_mem_size();
            res.GPUInfo[gpuName] = gpuMemSize;
        }

        multimap<string, Resource> resMap;
        resMap.insert(std::pair<string, Resource>(getNCIP(), res));
        setResourceMap(resMap);
    }
    else
    {
        return FAILED;
    }

    return SUCCESSFUL;
}

int NCRegTask::sendAckToNC(int ret)
{
    MsgHeader msg;
    setDataString("");

    msg.cmd = MSG_NC_RC_REGISTER_ACK;
    msg.length = 0;
    msg.error = ret;
    msg.para1 = m_NCTaskID;
    msg.para2 = m_NCTaskID >> 32;

    NCAgent *pAgent = dynamic_cast<NCAgent*>(
            (AgentManager::getInstance())->get(m_NCAgentID));
    if(pAgent == NULL)
    {
        ERROR_LOG(
                "NCRegTask::sendAckToNC: not found ncagent");
        return FAILED;
    }
    else
        return pAgent->sendPackage(msg);
}

void NCRegTask::clearTaskPara()
{
    m_NCIP.clear();
    setDataString("");
}

}

#include "NCRegTask.h"
#include "protocol/RcNcProtocol.pb.h"
#include "ResourceManager.h"
#include "ALAgent.h"
#include "FWMAgent.h"
#include "NCAgent.h"
#include "ConfigManager.h"
#include "protocol/RASCmdCode.h"
#include "protocol/TaskType.h"
#include "head.h"

#include "common/log/log.h"
#include "common/comm/TaskManager.h"
#include "common/comm/TCPListenAgent.h"
#include "common/comm/AgentManager.h"

namespace rc
{

NCRegTask::NCRegTask()
{
    #ifdef DEBUG
    INFO_LOG("NCRegTask constructed");
    #endif

    setTaskState(NCREGTASK_DOPARSE);
    setDataString("");
    setTaskType(NC_REG_TASK);
}

NCRegTask::~NCRegTask()
{
    m_NCIP.clear();
}

int NCRegTask::goNext()
{
    int ret = 0;
    switch(getTaskState())
    {
        case NCREGTASK_DOPARSE:
        {
            #ifdef DEBUG
            INFO_LOG("NCREGTASK_DOPARSE");
            #endif

            ret = doParse();
            setTaskState(NCREGTASK_REGISTER);
            ret = goNext();
            break;
        }
        case NCREGTASK_REGISTER:
        {
            #ifdef DEBUG
            INFO_LOG("NCREGTASK_REGISTER");
            INFO_LOG("NCRegTask::goNext: tid is %llu", getID());
            #endif

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
            #ifdef DEBUG
            INFO_LOG("NCREGTASK_CHECK_SERVICE");
            #endif

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
            #ifdef DEBUG
            INFO_LOG("NCREGTASK_CREATE_ALFWMLISTEN");
            #endif

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
            #ifdef DEBUG
            INFO_LOG("NCRegTask finished!");
            #endif

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
        NCAgent *pAgent = dynamic_cast<NCAgent*>(
                (AgentManager::getInstance())->get(m_NCAgentID));
        if(pAgent == NULL)
        {
            INFO_LOG("NCRegTask::doParse: NCAgent not found");
            return FAILED;
        }
        pAgent->setIP(ncReg.nc_ip());
        pAgent->getInfoFromNCToRC()->setIP(ncReg.nc_ip());


        #ifdef DEBUG
        INFO_LOG("NC register ip is %s, port is %d", 
                ncReg.nc_ip().c_str(), ncReg.nc_port());
        #endif

        Resource res;

        RcNcProto::ResourceInfo resInfo = ncReg.machine_total_resource();
        res.logicCPUNum = resInfo.cpu_num();
        res.cpuMemSize = resInfo.cpu_mem_size();
        INFO_LOG("NCRegTask::doParse: cpu is %f, mem is %d",
                res.logicCPUNum, res.cpuMemSize);

        for(int j = 0; j < resInfo.gpu_resource_info_size(); j++)
        {
            RcNcProto::GpuResourceInfo gpuResInfo = resInfo.gpu_resource_info(j);
            string gpuName = gpuResInfo.gpu_name();
            uint32_t gpuMemSize = gpuResInfo.gpu_mem_size();
            res.GPUInfo[gpuName] = gpuMemSize;
            INFO_LOG("NC register gpu name is %s, gpu_mem is %d", 
                    gpuName.c_str(), gpuMemSize);
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
    {
#ifdef DEBUG
        INFO_LOG("NCRegTask::sendAckToNC: send message to nc, cmd is %x", msg.cmd);
#endif
        return pAgent->sendPackage(msg);
    }
}

void NCRegTask::clearTaskPara()
{
    m_NCIP.clear();
    setDataString("");
}

}

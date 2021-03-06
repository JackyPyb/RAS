#include "InfoSendToNC.h"
#include "NCAgent.h"
#include "protocol/RASCmdCode.h"
#include "protocol/TaskType.h"
#include "StartFWRootTask.h"
#include "ConfigManager.h"
#include "protocol/RcNcProtocol.pb.h"
#include "FWIManager.h"
#include "FWInstance.h"
#include "head.h"

#include "common/comm/AgentManager.h"
#include "common/log/log.h"

namespace rc
{

class NCAgent;

InfoSendToNC::InfoSendToNC(uint32_t aid):
    m_NCAgentID(aid),
    m_data("")
{
 
}

InfoSendToNC::~InfoSendToNC()
{
    m_data.clear();
}

int InfoSendToNC::sendTaskToNC(Task *pTask, uint32_t agentID)
{
    uint32_t taskType = pTask->getTaskType();
    string data = "";
    MsgHeader msgHeader;

    switch(taskType)
    {
        case START_FW_ROOT_TASK:
        {
            StartFWRootTask *pStartRootTask = 
                dynamic_cast<StartFWRootTask*>(pTask);

            RcNcProto::StartFrameworkRoot startRootInfo;
            
            RcNcProto::FrameworkInstanceInfo *fwInstance = 
                startRootInfo.mutable_framework_instance_info();
            fwInstance->set_framework_id( 
                (pStartRootTask->m_startRootModuleInfo).framework_id());
            fwInstance->set_framework_instance_id( 
                pStartRootTask->getFWInstanceID());
            startRootInfo.set_self_module_id(pStartRootTask->getRootModuleID());
            RcNcProto::DockerImageInfo *dockerInfo = 
                startRootInfo.mutable_docker_image_info();
            dockerInfo->set_tag(
                    (pStartRootTask->m_startRootModuleInfo).image_lable());
            dockerInfo->set_locate_file(
                    (pStartRootTask->m_startRootModuleInfo).location_file_path());
            dockerInfo->set_module_name(
                    (pStartRootTask->m_startRootModuleInfo).module_name());
            RcNcProto::ResourceInfo *resInfo = 
                startRootInfo.mutable_require_resource();

            FWInstance * pFWInstance = (FWIManager::getInstance())->
                get(pStartRootTask->getFWInstanceID());
            resInfo->set_cpu_num(pFWInstance->getRootLogicCPUNum());
            resInfo->set_cpu_mem_size(pFWInstance->getRootMemSize());
            startRootInfo.set_listen_num(
                    (pStartRootTask->m_startRootModuleInfo).listen_num());
            RcNcProto::NetAddress *netAddress = startRootInfo.mutable_rc_address();
            netAddress->set_ip((ConfigManager::getInstance())
                    ->getLocalIP());
            SocketAddress rcFWMaddress = (ConfigManager::getInstance()
                    ->getFWMListenAddr());
            netAddress->set_port(rcFWMaddress.getPort());

#ifdef DEBUG
            INFO_LOG("InfoSendToNC::sendTaskToNC: START_FW_ROOT_TASK");
            INFO_LOG("FW id %d", (pStartRootTask->m_startRootModuleInfo).framework_id());
            INFO_LOG("FW instance id %d", pStartRootTask->getFWInstanceID());
            INFO_LOG("Self module id %d", pStartRootTask->getRootModuleID());
            INFO_LOG("Module name %s", (pStartRootTask->m_startRootModuleInfo).module_name().c_str());
            INFO_LOG("logicCPUNum %f", pFWInstance->getRootLogicCPUNum());
            INFO_LOG("cpuMemSize %d", pFWInstance->getRootMemSize());
            INFO_LOG("listen num %d", (pStartRootTask->m_startRootModuleInfo).listen_num());
            INFO_LOG("RC IP %s", ConfigManager::getInstance()->getLocalIP().c_str());
            INFO_LOG("RC port %d", ConfigManager::getInstance()->getFWMListenAddr().getPort());
            INFO_LOG("StartFWRootTask task id is %d", pStartRootTask->getID());
#endif

            startRootInfo.SerializeToString(&data);

            setMsgHeader( msgHeader, MSG_RC_NC_START_FRAMEWORK_ROOT,
                    data.length(),
                    pStartRootTask->getID());
            break;            
        }
        default:
            ERROR_LOG("UNKNOWN TASKTYPE");
            break;
    }

    NCAgent *pAgent = dynamic_cast<NCAgent*>(
            (AgentManager::getInstance())->get(agentID));

    if(pAgent == NULL)
    {
        ERROR_LOG("InfoSendToNC::sendTaskToNC: ncagent can not found");
        return FAILED;
    }

    return pAgent->sendPackage(msgHeader, data);
}

void InfoSendToNC::setMsgHeader(MsgHeader &msgHeader, uint32_t cmd, 
        uint32_t len, uint64_t tid)
{
    msgHeader.cmd = cmd;
    msgHeader.length = len;
    msgHeader.para1 = tid;
    msgHeader.para2 = tid >> 32;
}

}

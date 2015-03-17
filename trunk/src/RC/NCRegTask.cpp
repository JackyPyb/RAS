#include "NCRegTask.h"
#include "protocol/RcNcProtocol.pb.h"

#include "common/log/log.h"

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
    return SUCCESSFUL;
}

void NCRegTask::clearTaskPara()
{

}

}

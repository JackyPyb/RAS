#include "FWInstance.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

FWInstance::FWInstance():
    m_FWInstanceID(0)
{

}

FWInstance::FWInstance(const uint32_t id):
    m_FWInstanceID(id)
{

}

void FWInstance::setFWInstanceID(const uint32_t id)
{
    m_FWInstanceID = id;
}

uint32_t FWInstance::getFWInstanceID() const
{
    return m_FWInstanceID;
}

FWInstance::~FWInstance()
{
    m_eachNCApplyRes.clear();
    m_eachNCActualUseRes.clear();
}

void FWInstance::setQueryEntry(const string &ip, const uint32_t port)
{
    m_queryEntry.setIP(ip);
    m_queryEntry.setPort(port);
}

QueryEntry FWInstance::getQueryEntry() const
{
    return m_queryEntry;
}

void FWInstance::changeNCApplyRes(const string &ip, const Resource &applyRes)
{
    NCResIter ncApplyResIter = m_eachNCApplyRes.find(ip);
    if(ncApplyResIter != m_eachNCApplyRes.end())
    {
        m_totalApplyRes -= ncApplyResIter->second;
        m_totalApplyRes += applyRes;
    }
    else
    {
        m_totalApplyRes += applyRes;
    }

    m_eachNCApplyRes[ip] = applyRes;
}

bool FWInstance::addNCApplyRes(const string &ip, const Resource &addApplyRes)
{
    NCResIter ncApplyResIter = m_eachNCApplyRes.find(ip);
    if(ncApplyResIter == m_eachNCApplyRes.end())
    {
        m_eachNCApplyRes[ip] = addApplyRes;
    }
    else
    {
        m_eachNCApplyRes[ip] += addApplyRes;
    }

    m_totalApplyRes += addApplyRes;
    return SUCCESSFUL;
}

bool FWInstance::delNCApplyRes(const string &ip, const Resource &delApplyRes)
{
    NCResIter ncApplyResIter = m_eachNCApplyRes.find(ip);
    if(ncApplyResIter == m_eachNCApplyRes.end())
    {
        INFO_LOG("FWInstance::delNCApplyRes: %s did not in m_eachNCApplyRes", 
                ip.c_str());
        return FAILED;
    }
    else
    {
        m_totalApplyRes -= delApplyRes;

        if(ncApplyResIter->second >= delApplyRes)
        {
            ncApplyResIter->second -= delApplyRes;
            if(ncApplyResIter->second.isResZero())
            {
                m_eachNCApplyRes.erase(ncApplyResIter);
                return SUCCESSFUL;
            }
        }
        else
        {
            INFO_LOG("FWInstance::delNCApplyRes: \
                    delApplyRes exceed applied res in %s", ip.c_str());
            return FAILED;
        }   
    }
}

Resource FWInstance::getNCApplyRes(const string &ip)
{
    NCResIter ncApplyResIter = m_eachNCApplyRes.find(ip); 
    if(ncApplyResIter != m_eachNCApplyRes.end())
    {
        return ncApplyResIter->second;
    }
    else
    {
        return Resource();
    }
}

void FWInstance::setNCActualUseRes(const string &ip, 
        const Resource &actualUseRes)
{
    m_eachNCActualUseRes[ip] = actualUseRes;
}

Resource FWInstance::getNCActualUseRes(const string &ip) const
{
    NCResConstIter ncActualUseIter = m_eachNCActualUseRes.find(ip);
    if(ncActualUseIter != m_eachNCActualUseRes.end())
    {
        return ncActualUseIter->second;
    }
    else
    {
        return Resource();
    }
}

void FWInstance::setEachNCApplyRes(const map<string, Resource> &resMap)
{
    NCResConstIter resIter = resMap.begin();
    for(; resIter != resMap.end(); resIter++)
    {
        string ip = resIter->first;
        NCResIter applyIter = m_eachNCApplyRes.find(ip);
        if(applyIter != m_eachNCApplyRes.end())
        {
            m_totalApplyRes -= applyIter->second;
            m_totalApplyRes += resIter->second;
        }
        else
        {
            m_totalApplyRes += resIter->second;
        }
    }

    m_eachNCApplyRes= resMap;
}

map<string, Resource> FWInstance::getEachNCApplyRes() const
{
    return m_eachNCApplyRes;
}

void FWInstance::setEachNCActualUseRes(const map<string, Resource> &resMap)
{
    m_eachNCActualUseRes = resMap;
}

map<string, Resource> FWInstance::getEachNCActualUseRes() const
{
    return m_eachNCActualUseRes;
}

void FWInstance::setTotalApplyRes(const Resource &res)
{   
    m_totalApplyRes = res;
}

Resource FWInstance::getTotalApplyRes() const
{
    return m_totalApplyRes;
}

void FWInstance::setTotalActualUseRes(const Resource &res)
{
    m_totalActualUseRes = res;
}

Resource FWInstance::getTotalActualUseRes() const
{
    return m_totalActualUseRes;
}

bool FWInstance::addRootModule(const uint32_t moduleID)
{
    ParentChildrenIter parentChildrenIter = m_moduleParentChildrenMap.find(moduleID);
    if(parentChildrenIter != m_moduleParentChildrenMap.end())
    {
        INFO_LOG("FWInstance::addRootModule: \
                module %d is already in FWInstance %d", 
                moduleID, m_FWInstanceID);
        return false;
    }
    else
    {
        set<uint32_t> childrenSet;
        m_moduleParentChildrenMap[moduleID] = childrenSet;
        this->setRootModuleID(moduleID);
        return true;        
    }    
}

bool FWInstance::addModule(const uint32_t parentID, const uint32_t childID)
{
    ParentChildrenIter parentChildrenIter = 
        m_moduleParentChildrenMap.find(parentID);    
    if(parentChildrenIter == m_moduleParentChildrenMap.end())
    {
        INFO_LOG("FWInstance::addModule: \
                parent module %d is not in FWInstance %d",
                parentID, m_FWInstanceID);
        return false;
    }
    else
    {
        SetIter childrenIter = (parentChildrenIter->second).find(childID);
        if(childrenIter != (parentChildrenIter->second).end())
        {
            INFO_LOG("FWInstance::addModule: \
                    childID %d is already in parent %d in FWInstance %d", 
                    childID, parentID, m_FWInstanceID);
            return false;
        }
        else
        {
            (parentChildrenIter->second).insert(childID);
        }
    }

    return true;
}

bool FWInstance::delModule(const uint32_t id)
{
    ParentChildrenIter parentChildrenIter = 
        m_moduleParentChildrenMap.find(id);
    if(parentChildrenIter == m_moduleParentChildrenMap.end())
    {
        INFO_LOG("FWInstance::delModule: \
                module %d is not in FWInstance %d",
                id, m_FWInstanceID);
        return false;
    }
    else
    {
        if(!(parentChildrenIter->second).empty())
        {
            SetIter setIter = (parentChildrenIter->second).begin();
            for(; setIter != (parentChildrenIter->second).end(); setIter++)
            {
                this->delModule(*setIter);
                (parentChildrenIter->second).erase(*setIter);
            }
        }
        else
        {
            m_moduleParentChildrenMap.erase(id);
            m_moduleIDIPProcessMap.erase(id);
        }
    }

    return true;
}

bool FWInstance::delAllModules(const uint32_t rootModuleID)
{
    return delModule(getRootModuleID());
}

void FWInstance::setModuleIPProcess(const uint32_t moduleID,
        const string &ip, uint32_t processID)
{
    IPProcess ipProcess(ip, processID);
    m_moduleIDIPProcessMap[moduleID] = ipProcess;
}

IPProcess FWInstance::getModuleIPProcess(const uint32_t moduleID) const
{
    IDIPProcessConstIter idIPProcessIter = m_moduleIDIPProcessMap.find(moduleID);
    if(idIPProcessIter != m_moduleIDIPProcessMap.end())
    {
        return idIPProcessIter->second;
    }
    else
    {
        return IPProcess();
    }
}

void FWInstance::setRootModuleID(const uint32_t rootModuleID)
{
    m_rootModuleID = rootModuleID;
}

uint32_t FWInstance::getRootModuleID() const
{
    return m_rootModuleID;
}

void FWInstance::setRootLogicCPUNum(const double logicCPUNum)
{
    m_rootLogicCPUNum = logicCPUNum;
}

double FWInstance::getRootLogicCPUNum() const
{
    return m_rootLogicCPUNum;
}

void FWInstance::setRootMemSize(const uint32_t memSize)
{
    m_rootMemSize = memSize;
}

uint32_t FWInstance::getRootMemSize() const
{
    return m_rootMemSize;
}

void FWInstance::setRootGPUInfo(const multimap<string, uint32_t> &gpuInfo)
{
    m_rootGPUInfo = gpuInfo;    
}

multimap<string, uint32_t> FWInstance::getRootGPUInfo() const
{
    return m_rootGPUInfo;
}

void FWInstance::setIsClosing(bool isClosing)
{
    m_isClosing = isClosing;
}

bool FWInstance::getIsClosing() const
{
    return m_isClosing;
}

}

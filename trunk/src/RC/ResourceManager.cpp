#include "ResourceManager.h"
#include "ConfigManager.h"
#include "NCRegTask.h"
#include "head.h"

#include "protocol/RASErrorCode.h"
#include "common/comm/AgentManager.h"
#include "common/comm/TaskManager.h"
#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

ResourceManager::ResourceManager():
    m_LBNum(0),
    m_isALFWMListenCreated(false)
{

}

ResourceManager::~ResourceManager()
{
    NCMapIter ncMapIter = m_NCMap.begin();
    for(; ncMapIter != m_NCMap.end(); ncMapIter++)
    {
        delete ncMapIter->second;
        ncMapIter->second = NULL;
    }

    m_NCMap.clear();
    m_memFirstGPUNotConsider.clear();
    m_CPUFirstGPUNotConsider.clear();
}

NCLoadBalance* ResourceManager::createNCLB(const string &ip, const uint32_t tid)
{
    NCLoadBalance *pNCLB = new NCLoadBalance(ip);

    Resource res;
    NCRegTask * pNCRegTask = dynamic_cast<NCRegTask*>((TaskManager::getInstance())->get(tid));
    if(pNCRegTask != NULL)
    {
        multimap<string, Resource> resMap = pNCRegTask->getResourceMap();
        multimap<string, Resource>::iterator iter = resMap.find(ip);
        res = iter->second;
        pNCLB->setTotalNCRes(res);
        
        //to test
        res = pNCLB->getTotalNCRes();
        INFO_LOG("add to nc %s, total res cpu is %f, mem is %d",
                ip.c_str(), res.logicCPUNum, res.cpuMemSize);
    }

    return pNCLB;
}

int ResourceManager::registerNC(const string &ip, const uint32_t aid, const uint32_t tid)
{
    NCLoadBalance *pNCLB = createNCLB(ip, tid);
    bool ret = addNCLB(ip, pNCLB);
    if(ret == false)
    {
        ERROR_LOG(
                "ResourceManager::registerNC: %s already in", ip.c_str());
        return FAILED;
    }

    addPlatformTotalRes(pNCLB->getTotalNCRes());

    //to test
    Resource res = getPlatformTotalRes();
    INFO_LOG("Platform total res, cpu is %f, mem is %d",
            res.logicCPUNum, res.cpuMemSize);

    addNCLBToSets(ip);

    if(!pNCLB->isNCOnline())
    {
        pNCLB->registerOn(aid);
    }

    m_LBNum++;
    INFO_LOG("NC NUM is %d now", m_LBNum);

    return SUCCESSFUL;
}

int ResourceManager::NCOffLine(const string &ip)
{
    NCLoadBalance *pNCLB = getNCLB(ip);
    if(pNCLB == NULL)
        return FAILED;

    if(pNCLB->isNCOnline())
    {
        INFO_LOG(
                "ResourceManager::NCOffline: %s is offline", ip.c_str());
        pNCLB->setOnline(false);
        m_LBNum--;
        return (deleteNCLBInSets(ip) && deleteNCLB(ip));
    }

    return SUCCESSFUL;
}

bool ResourceManager::addNCLBToSets(const string &ip)
{
    NCLoadBalance* pNCLB = m_NCMap.find(ip)->second;
    NCInfo ncInfo(ip, pNCLB);

    m_memFirstGPUNotConsider.insert(ncInfo);
    m_CPUFirstGPUNotConsider.insert(ncInfo);
    return SUCCESSFUL;
}

bool ResourceManager::deleteNCLBInSets(const string &ip)
{
    NCLoadBalance* pNCLB = m_NCMap.find(ip)->second;
    NCInfo ncInfo(ip, pNCLB);

    std::pair<MemSetIter, MemSetIter> memRet = 
        m_memFirstGPUNotConsider.equal_range(ncInfo);

    for(; memRet.first != memRet.second; (memRet.first)++)
    {
        if(((memRet.first)->getIP()).compare(ip) == 0)
        {
            m_memFirstGPUNotConsider.erase(memRet.first);
        }
    }
    
    std::pair<CPUSetIter, CPUSetIter> cpuRet = 
        m_CPUFirstGPUNotConsider.equal_range(ncInfo);

    for(; cpuRet.first != cpuRet.second; (cpuRet.first)++)
    {
        if(((cpuRet.first)->getIP()).compare(ip) == 0)
        {
            m_CPUFirstGPUNotConsider.erase(cpuRet.first);
        }
    }

    return SUCCESSFUL;
}

bool ResourceManager::changeNCLBInSets(const string &ip)
{
    return SUCCESSFUL;
}

bool ResourceManager::addNCLB(const string &ip, NCLoadBalance* ncLoadBalance)
{
    bool ret = false;
    NCMapIter ncMapIter = m_NCMap.find(ip);
    if(ncMapIter == m_NCMap.end())
    {
        ret = true;
        m_NCMap[ip] = ncLoadBalance;
    }

    return ret;
}

bool ResourceManager::deleteNCLB(const string &ip)
{
    bool ret = false;
    NCMapIter ncMapIter = m_NCMap.find(ip);
    if(ncMapIter != m_NCMap.end())
    {
        ret = true;
        delete ncMapIter->second;
        ncMapIter->second = NULL;
        m_NCMap.erase(ncMapIter);
    }

    return ret;
}

NCLoadBalance* ResourceManager::getNCLB(const string &ip)
{
    NCLoadBalance * ncLoadBalance = NULL;
    NCMapIter ncMapIter = m_NCMap.find(ip);
    if(ncMapIter != m_NCMap.end())
    {
        ncLoadBalance = ncMapIter->second;
    }

    return ncLoadBalance;
}

bool ResourceManager::findNCLB(const string &ip)
{
    bool ret = false;
    NCMapIter ncMapIter = m_NCMap.find(ip);
    if(ncMapIter != m_NCMap.end())
    {
        ret = true;
    }

    return ret;
}

uint32_t ResourceManager::getLBNum() const
{
    return m_LBNum;
}

string ResourceManager::getSuitableNCByMem(const Resource &res) const
{
    string suitableIP = "";
    MemSetIter iter = m_memFirstGPUNotConsider.begin();
    for(; iter != m_memFirstGPUNotConsider.end(); iter++)
    {
        NCInfo ncInfo = *iter;
        NCLoadBalance * pNCLB = ncInfo.getNCLB();
        Resource ncRes = pNCLB->getNotApplyRes();
        if((ncRes.cpuMemSize >= res.cpuMemSize) &&
                (ncRes.logicCPUNum >= res.logicCPUNum))
        {
            suitableIP = ncInfo.getIP();
            break;
        }
        else if((ncRes.cpuMemSize < res.cpuMemSize) &&
                (ncRes.logicCPUNum < res.logicCPUNum))
        {
            break;
        }
        else
        {
            continue;
        }
    }
    
#ifdef DEBUG
    INFO_LOG("ResourceManager::getSuitableNCByMem");
    printMemSet();
    INFO_LOG("Finally choose %s", suitableIP.c_str());

#endif

    return suitableIP;
}

void ResourceManager::printMemSet() const
{
    MemSetIter debugIter = m_memFirstGPUNotConsider.begin();
    for(; debugIter != m_memFirstGPUNotConsider.end(); debugIter++)
    {
        NCInfo debugNCInfo = *debugIter;
        NCLoadBalance *pDebugNCLB = debugNCInfo.getNCLB();
        Resource debugNCRes = pDebugNCLB->getNotApplyRes();
        string debugIP = debugNCInfo.getIP();
        INFO_LOG("IP %s, logicCPUNum is %f, cpuMemSize is %d",
                debugIP.c_str(), debugNCRes.logicCPUNum, debugNCRes.cpuMemSize);
    }
}

string ResourceManager::getSuitableNCByCPU(const Resource &res) const
{
    string suitableIP = "";
    CPUSetIter iter = m_CPUFirstGPUNotConsider.begin();
    for(; iter != m_CPUFirstGPUNotConsider.end(); iter++)
    {
        NCInfo ncInfo = *iter;
        NCLoadBalance * pNCLB = ncInfo.getNCLB();
        Resource ncRes = pNCLB->getNotApplyRes();
        if((ncRes.cpuMemSize >= res.cpuMemSize) &&
                (ncRes.logicCPUNum >= res.logicCPUNum))
        {
            suitableIP = ncInfo.getIP();
            break;
        }
        else if((ncRes.cpuMemSize < res.cpuMemSize) &&
                (ncRes.logicCPUNum < res.logicCPUNum))
        {
            break;
        }
        else
        {
            continue;
        }
    }

#ifdef DEBUG
    INFO_LOG("ResourceManager::getSuitableNCByCPU");
    printCPUSet();
    INFO_LOG("Finally choose %s", suitableIP.c_str());
#endif

    return suitableIP;
}

void ResourceManager::printCPUSet() const
{
    CPUSetIter debugIter = m_CPUFirstGPUNotConsider.begin();
    for(; debugIter != m_CPUFirstGPUNotConsider.end(); debugIter++)
    {
        NCInfo debugNCInfo = *debugIter;
        NCLoadBalance *pDebugNCLB = debugNCInfo.getNCLB();
        Resource debugNCRes = pDebugNCLB->getNotApplyRes();
        INFO_LOG("IP %s, logicCPUNum is %f, cpuMemSize is %d",
                debugNCInfo.getIP().c_str(),
                debugNCRes.logicCPUNum,
                debugNCRes.cpuMemSize);
    }
}

string ResourceManager::getSuitableNCFromVec(const vector<string> &ips) const
{
    return string("");
}

//void ResourceManager::sortNCLoadBalance()
//{
//    string ip = m_NCMapIterNow->first;
//    deleteNCLBInSets(ip);
//    addNCLBToSets(ip);
//}

int ResourceManager::dealNCCrash(const string &ip)
{
    return SUCCESSFUL;
}

bool ResourceManager::checkServiceIsOK() const
{
    if(m_LBNum >= (ConfigManager::getInstance())->getMinNCNum())
    {
        INFO_LOG("service is OK");
        return true;
    }
    else
    {
        INFO_LOG("service is not OK");
        return false;
    }
}

void ResourceManager::setALFWMListenCreated(bool isListened)
{
    #ifdef DEBUG
    INFO_LOG("ResourceManager::setALFWMListenCreated %d", isListened);
    #endif
    m_isALFWMListenCreated = isListened;
}

bool ResourceManager::isALFWMListenCreated() const
{
    return m_isALFWMListenCreated;
}

void ResourceManager::setPlatformTotalRes(const Resource &res)
{
    m_platformTotalRes = res;
}

Resource ResourceManager::getPlatformTotalRes() const
{
    return m_platformTotalRes;
}

bool ResourceManager::addPlatformTotalRes(const Resource &res)
{
    m_platformTotalRes += res;
    return SUCCESSFUL;
}

bool ResourceManager::delPlatformTotalRes(const Resource &res)
{
    if(m_platformTotalRes >= res)
    {
        m_platformTotalRes -= res;
    }
    else
    {
        ERROR_LOG(
                "ResourceManager::delPlatformTotalRes: \
                del resource exceeds platform total resource");
        return FAILED;
    }

    return SUCCESSFUL;
}

}

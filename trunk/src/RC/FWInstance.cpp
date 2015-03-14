#include "FWInstance.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

FWInstance::FWInstance()
{

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

}

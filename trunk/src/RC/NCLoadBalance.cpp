#include <algorithm>

#include "NCLoadBalance.h"
#include "head.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

namespace rc
{

NCLoadBalance::NCLoadBalance():
    m_isOnline(false),
    m_NCIP(""),
    m_NCPort(0),
    m_needSort(false),
    m_pInfoSendToNC(NULL)
{

}


NCLoadBalance::NCLoadBalance(string ip):
    m_isOnline(false),
    m_NCIP(ip),
    m_NCPort(0),
    m_needSort(false),
    m_pInfoSendToNC(NULL)
{

}

NCLoadBalance::~NCLoadBalance()
{
#ifdef DEBUG
    INFO_LOG("NCLoadBalance::~NCLoadBalance");
#endif
    m_NCIP.clear();
    if(m_pInfoSendToNC != NULL)
    {
        delete m_pInfoSendToNC;
        m_pInfoSendToNC = NULL;
    }
}

void NCLoadBalance::registerOn(const uint32_t aid)
{
    setNCAgentID(aid);
    setOnline(true);
    m_pInfoSendToNC = new InfoSendToNC(m_NCAgentID);
}

//int NCLoadBalance::checkForAddTask(Task* pTask)
//{
//    return SUCCESSFUL;
//}

int NCLoadBalance::sendTaskToNC(Task *pTask)
{
    return m_pInfoSendToNC->sendTaskToNC(pTask);
}

void NCLoadBalance::setFWInstance(const set<uint32_t>& FWInstanceSet)
{
    m_FWInstanceSet = FWInstanceSet;
}

bool NCLoadBalance::addFWInstance(const uint32_t FWInstance)
{
    m_FWInstanceSet.insert(FWInstance);
    return SUCCESSFUL;
}

bool NCLoadBalance::delFWInstance(const uint32_t FWInstance)
{
    bool ret = false;
    SetIter setIter = find(m_FWInstanceSet.begin(), m_FWInstanceSet.end(), 
            FWInstance);
    if(setIter != m_FWInstanceSet.end())
    {
        ret = true;
        m_FWInstanceSet.erase(setIter);
    }

    return ret;
}

set<uint32_t> NCLoadBalance::getFWInstance() const
{
    return m_FWInstanceSet;
}

void NCLoadBalance::setTotalNCRes(const Resource &totalRes)
{
    m_totalNCRes = totalRes;
    m_notApplyRes = m_totalNCRes;
}

Resource NCLoadBalance::getTotalNCRes() const
{
    return m_totalNCRes;
}

bool NCLoadBalance::addApplyRes(const Resource &res)
{
    bool ret = false;
    if((this->getNotApplyRes()) < res)
    {
        INFO_LOG("NCLoadBalance::addApplyRes: res exceed not applied res");
        return ret;
    }

    m_applyRes += res;
    m_notApplyRes -= res;

    ret = true;
    return ret;
}

bool NCLoadBalance::delApplyRes(const Resource &res)
{   
    m_applyRes -= res;
    m_notApplyRes += res;
    return true;
}

Resource NCLoadBalance::getApplyRes() const
{
    return m_applyRes;
}

Resource NCLoadBalance::getNotApplyRes() const
{
    return m_notApplyRes;
}

bool NCLoadBalance::setActualUseRes(const Resource &res)
{
    bool ret = false;
    if((this->getTotalNCRes()) < res)
    {
        INFO_LOG("NCLoadBalance::setActualUseRes: res exceed totalNCRes");
        return ret;
    }

    m_actualUseRes = res;

    ret = true;
    return ret;
}

Resource NCLoadBalance::getActualUseRes() const
{
    return m_actualUseRes;
}

bool NCLoadBalance::setActualRemainRes(const Resource &res)
{
    m_actualRemainRes = res;
    return true;
}

Resource NCLoadBalance::getActualRemainRes() const
{
    return m_actualRemainRes;
}

}

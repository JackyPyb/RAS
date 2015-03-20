#ifndef _RC_NCLOADBALANCE_H_
#define _RC_NCLOADBALANCE_H_

#include <string>
#include <set>
#include <stdint.h>

#include "common/comm/TaskManager.h"

#include "InfoSendToNC.h"
#include "Resource.h"

using std::string;
using std::set;

namespace rc
{
class NCLoadBalance
{
public:
    NCLoadBalance();
    NCLoadBalance(string);
    ~NCLoadBalance();

    void setOnline(const bool online)
    {
        m_isOnline = online;
    }

    bool isNCOnline() const
    {
        return m_isOnline;
    }

    void setNCIP(const string &ip)
    {
        m_NCIP = ip;
    }

    string getNCIP() const
    {
        return m_NCIP;
    }

    void setNCPort(const uint32_t port)
    {
        m_NCPort = port;
    }

    uint32_t getNCPort() const
    {
        return m_NCPort;
    }

    void setNCAgentID(const uint32_t aid)
    {
        m_NCAgentID = aid;
    }

    uint32_t getNCAgentID() const
    {
        return m_NCAgentID;
    }

    bool needToSort() const
    {
        return m_needSort;
    }

    void registerOn(const uint32_t);
//    int checkForAddTask(Task*);
    int sendTaskToNC(Task*);
    void setFWInstance(const set<uint32_t>&);
    bool addFWInstance(const uint32_t);
    bool delFWInstance(const uint32_t);
    set<uint32_t> getFWInstance() const;

    void setTotalNCRes(const Resource&);
    Resource getTotalNCRes() const;
    bool addApplyRes(const Resource&);
    bool delApplyRes(const Resource&);
    Resource getApplyRes() const;
    Resource getNotApplyRes() const;
    bool setActualUseRes(const Resource&);
    Resource getActualUseRes() const;
    bool setActualRemainRes(const Resource&);
    Resource getActualRemainRes() const;

private:
    typedef set<uint32_t>::iterator SetIter;

    bool m_isOnline;
    string m_NCIP;
    uint32_t m_NCPort;
    uint32_t m_NCAgentID;
    bool m_needSort;
    set<uint32_t> m_FWInstanceSet;
    InfoSendToNC *m_pInfoSendToNC;

    Resource m_totalNCRes;
    Resource m_applyRes;
    Resource m_notApplyRes;
    Resource m_actualUseRes;
    Resource m_actualRemainRes;
};

}

#endif

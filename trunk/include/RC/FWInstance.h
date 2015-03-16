#ifndef _RC_FWINSTANCE_H_
#define _RC_FWINSTANCE_H_

#include <string>
#include <map>
#include <set>

using std::map;
using std::multimap;
using std::string;
using std::set;

#include "Resource.h"

namespace rc
{

struct QueryEntry
{
public:
    QueryEntry():
        m_IP(""),
        m_port(0)
    {

    }

    QueryEntry(const string &ip, const uint32_t port):
        m_IP(ip),
        m_port(port)
    {

    }

    ~QueryEntry()
    {
        m_IP.clear();
    }

    void setIP(const string &ip)
    {
        m_IP = ip;
    }

    string getIP() const
    {
        return m_IP;
    }

    void setPort(uint32_t port)
    {
        m_port = port;
    }

    uint32_t getPort() const
    {
        return m_port;
    }

private:
    string m_IP;
    uint32_t m_port;
};

struct IPProcess
{
public:
    IPProcess():
        m_IP(""),
        m_processID(0)
    {

    }

    IPProcess(const string &ip, const uint32_t processID):
        m_IP(ip),
        m_processID(processID)
    {

    }

    ~IPProcess()
    {
        m_IP.clear();
    }

    void setIP(const string &ip)
    {
        m_IP = ip;
    }

    string getIP() const
    {
        return m_IP;
    }

    void setProcessID(const uint32_t processID)
    {
        m_processID = processID;
    }

    uint32_t getProcessID() const
    {
        return m_processID;
    }

private:
    string m_IP;
    uint32_t m_processID;
};

class FWInstance
{
public:
    FWInstance();
    FWInstance(const uint32_t);
    ~FWInstance();

    void setFWInstanceID(const uint32_t);
    uint32_t getFWInstanceID() const;
    void setQueryEntry(const string &ip, const uint32_t port);
    QueryEntry getQueryEntry() const;
    void changeNCApplyRes(const string &ip, const Resource &applyRes);
    bool addNCApplyRes(const string &ip, const Resource &addApplyRes);
    bool delNCApplyRes(const string &ip, const Resource &delApplyRes);
    Resource getNCApplyRes(const string &ip);
    void setNCActualUseRes(const string &ip, const Resource &actualUseRes);
    Resource getNCActualUseRes(const string &ip) const;
    void setEachNCApplyRes(const map<string, Resource> &);
    map<string, Resource> getEachNCApplyRes() const;
    void setEachNCActualUseRes(const map<string, Resource> &);
    map<string, Resource> getEachNCActualUseRes() const;

    void setTotalApplyRes(const Resource &);
    Resource getTotalApplyRes() const;
    void setTotalActualUseRes(const Resource &);
    Resource getTotalActualUseRes() const;

    bool addRootModule(const uint32_t);
    bool addModule(const uint32_t, const uint32_t);
    bool delModule(const uint32_t);
    bool delAllModules(const uint32_t);
    void setModuleIPProcess(const uint32_t, const string &, uint32_t);
    IPProcess getModuleIPProcess(const uint32_t) const;

    void setRootModuleID(const uint32_t);
    uint32_t getRootModuleID() const;
    void setRootLogicCPUNum(const double);
    double getRootLogicCPUNum() const;
    void setRootMemSize(const uint32_t);
    uint32_t getRootMemSize() const;
    void setRootGPUInfo(const multimap<string, uint32_t> &);
    multimap<string, uint32_t> getRootGPUInfo() const;
    void setIsClosing(bool);
    bool getIsClosing() const;

private:
    typedef map<string, Resource>::iterator NCResIter;
    typedef map<string, Resource>::const_iterator NCResConstIter;
    typedef map<uint32_t, set<uint32_t> >::iterator ParentChildrenIter;
    typedef map<uint32_t, set<uint32_t> >::const_iterator ParentChildrenConstIter;
    typedef map<uint32_t, IPProcess>::iterator IDIPProcessIter;
    typedef map<uint32_t, IPProcess>::const_iterator IDIPProcessConstIter;
    typedef set<uint32_t>::iterator SetIter;
    typedef set<uint32_t>::const_iterator SetConstIter;

    uint32_t m_FWInstanceID;
    QueryEntry m_queryEntry;
    map<string, Resource> m_eachNCApplyRes;
    map<string, Resource> m_eachNCActualUseRes;
    Resource m_totalApplyRes;
    Resource m_totalActualUseRes;

    map<uint32_t, set<uint32_t> > m_moduleParentChildrenMap;
    map<uint32_t, IPProcess> m_moduleIDIPProcessMap;

    uint32_t m_rootModuleID;
    double m_rootLogicCPUNum;
    uint32_t m_rootMemSize;
    multimap<string, uint32_t> m_rootGPUInfo;
    bool m_isClosing;

};

}

#endif

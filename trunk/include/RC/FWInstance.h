#ifndef _RC_FWINSTANCE_H_
#define _RC_FWINSTANCE_H_

#include <string>
#include <map>

using std::map;
using std::string;

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

class FWInstance
{
public:
    FWInstance();
    ~FWInstance();

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

private:
    typedef map<string, Resource>::iterator NCResIter;
    typedef map<string, Resource>::const_iterator NCResConstIter;

    QueryEntry m_queryEntry;
    map<string, Resource> m_eachNCApplyRes;
    map<string, Resource> m_eachNCActualUseRes;
    Resource m_totalApplyRes;
    Resource m_totalActualUseRes;
};

}

#endif

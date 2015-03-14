#ifndef _RC_RESOURCEMANAGER_H_
#define _RC_RESOURCEMANAGER_H_

#include <stdint.h>
#include <map>
#include <set>
#include <vector>

#include "common/sys/Singleton.h"

#include "NCLoadBalance.h"

using std::map;
using std::multiset;
using std::vector;

namespace rc
{
struct NCInfo
{
public:
    NCInfo(string ip):
        m_NCIP(ip),
        m_pNCLB(NULL)
    {

    }

    NCInfo(string ip, NCLoadBalance* pNCLB):
        m_NCIP(ip),
        m_pNCLB(pNCLB)
    {

    }

    ~NCInfo()
    {
        m_NCIP.clear();
        if(m_pNCLB != NULL)
        {
            delete m_pNCLB;
            m_pNCLB = NULL;
        }
    }

    void setNCLB(NCLoadBalance *pNCLB)
    {
        delete m_pNCLB;
        m_pNCLB = pNCLB;
    }

    NCLoadBalance* getNCLB() const
    {
        return m_pNCLB;
    }

    void setIP(string ip)
    {
        m_NCIP = ip;
    }

    string getIP() const
    {
        return m_NCIP;
    }

    bool operator < (const NCInfo &ncInfo) const
    {
        return m_NCIP < ncInfo.m_NCIP;
    }

private:
    string m_NCIP;
    NCLoadBalance *m_pNCLB;

};


struct MemFirstCmp
{
    bool operator() (const NCInfo &ncInfo1, const NCInfo &ncInfo2) const
    {
        NCLoadBalance *loadBalance1 = ncInfo1.getNCLB();
        NCLoadBalance *loadBalance2 = ncInfo2.getNCLB();

        Resource notApplyRes1 = loadBalance1->getNotApplyRes();
        Resource notApplyRes2 = loadBalance2->getNotApplyRes();

        if(notApplyRes1.cpuMemSize > notApplyRes2.cpuMemSize)
        {
            return true;
        }
        else if(notApplyRes1.cpuMemSize == notApplyRes2.cpuMemSize)
        {
            if(notApplyRes1.logicCPUNum >= notApplyRes2.logicCPUNum)
            {
                return true; 
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
};


struct CPUFirstCmp
{
    bool operator() (const NCInfo &ncInfo1, const NCInfo &ncInfo2) const
    {
        NCLoadBalance *loadBalance1 = ncInfo1.getNCLB();
        NCLoadBalance *loadBalance2 = ncInfo2.getNCLB();

        Resource notApplyRes1 = loadBalance1->getNotApplyRes();
        Resource notApplyRes2 = loadBalance2->getNotApplyRes();

        if(notApplyRes1.logicCPUNum > notApplyRes2.logicCPUNum)
        {
            return true;
        }
        else if(notApplyRes1.logicCPUNum == notApplyRes2.logicCPUNum)
        {
            if(notApplyRes1.cpuMemSize >= notApplyRes2.cpuMemSize)
            {
                return true; 
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
};

class ResourceManager : public Singleton<ResourceManager>
{
protected:
    ResourceManager();
    ~ResourceManager();

public:
    friend class Singleton<ResourceManager>;

    NCLoadBalance* createNCLB(const string&);
    int registerNC(const string&, const uint32_t);
    int unregisterNC(const string&);
    int NCOffLine(const string&);

    bool addNCLBToSets(const string&);
    bool deleteNCLBInSets(const string&);
    bool changeNCLBInSets(const string&);

    bool addNCLB(const string&, NCLoadBalance*);
    bool deleteNCLB(const string&);
    NCLoadBalance* getNCLB(const string&);
    bool findNCLB(const string&);

    uint32_t getLBNum() const;
    string getSuitableNC() const;
    string getSuitableNCFromVec(const vector<string>&) const;
    void sortNCLoadBalance();
    int dealNCCrash(const string&);
    bool checkServiceIsOK() const;
    bool isFWMListenCreated() const;

private:
    typedef map<string, NCLoadBalance*>::iterator NCMapIter;
    typedef multiset<NCInfo, MemFirstCmp>::iterator MemSetIter;
    typedef multiset<NCInfo, CPUFirstCmp>::iterator CPUSetIter;

    map<string, NCLoadBalance*> m_NCMap;
    multiset<NCInfo, MemFirstCmp> m_memFirstGPUNotConsider;
    multiset<NCInfo, CPUFirstCmp> m_CPUFirstGPUNotConsider;
    uint32_t m_LBNum;
    bool m_isFWMListenCreated;

    NCMapIter m_NCMapIterNow;

};    

}

#endif

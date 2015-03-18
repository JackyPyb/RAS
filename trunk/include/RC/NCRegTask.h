#ifndef _RC_NCREGTASK_H_
#define _RC_NCREGTASK_H_

#include "ContainResourceTask.h"

#include <string>
#include <map>

using std::string;
using std::multimap;

namespace rc
{

const uint32_t NCREGTASK_DOPARSE = 1;
const uint32_t NCREGTASK_REGISTER = 2;
const uint32_t NCREGTASK_CHECK_SERVICE = 3;
const uint32_t NCREGTASK_CREATE_ALFWMLISTEN = 4;
const uint32_t NCREGTASK_FINISH_TASK = 5;

class NCRegTask : public ContainResourceTask
{
public:
    NCRegTask();
    ~NCRegTask();

    int goNext();
    int sendAckToNC(int);
    int doParse();
    void clearTaskPara();
    void setAgentID(uint32_t aid)
    {
        m_NCAgentID = aid;
    }

    uint32_t getAgentID() const
    {
        return m_NCAgentID;
    }

    void setNCIP(const string &ip)
    {
        m_NCIP = ip;
    }

    string getNCIP() const
    {
        return m_NCIP;
    }

    void setPort(uint32_t port)
    {
        m_port = port;
    }

    uint32_t getPort() const
    {
        return m_port;
    }

    void setNCTaskID(uint64_t ncTid)
    {
        m_NCTaskID = ncTid;
    }

    uint64_t getNCTaskID() const
    {
        return m_NCTaskID;
    }

private:
    uint32_t m_NCAgentID;
    string m_NCIP;
    uint32_t m_port;
    uint64_t m_NCTaskID;
};

}

#endif

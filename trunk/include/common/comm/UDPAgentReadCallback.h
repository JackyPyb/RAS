#ifndef UDPAGENTREADCALLBACK_H_
#define UDPAGENTREADCALLBACK_H_

#include "common/comm/ReadCallback.h"
#include "common/comm/UDPAgent.h"
#include "common/log/log.h"

class UDPAgentReadCallback : public ReadCallback
{
public:
    explicit UDPAgentReadCallback( UDPAgent* pAgent )
    {
        m_pAgent = pAgent;
    }

    virtual ~UDPAgentReadCallback()
    {
        m_pAgent = NULL;
    }

    virtual int run( InReq& req )
    {
        if ( NULL == m_pAgent )
        {
            ERROR_LOG(
                "In UDPAgentWriteCallback::run, m_pAgent == NULL" );
            return FAILED;
        }

        m_pAgent->readBack( req );
        return SUCCESSFUL;
    }

private:
    UDPAgent* m_pAgent;
};

#endif  // UDPAGENTREADCALLBACK_H_


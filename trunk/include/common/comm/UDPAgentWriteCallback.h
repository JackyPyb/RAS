#ifndef UDPAGENTWRITECALLBACK_H_
#define UDPAGENTWRITECALLBACK_H_

#include "common/comm/WriteCallback.h"
#include "common/comm/UDPAgent.h"
#include "common/log/log.h"

class UDPAgentWriteCallback : public WriteCallback
{
public:
    explicit UDPAgentWriteCallback(
        UDPAgent* pAgent )
    {
        m_pAgent = pAgent;
    }

    virtual ~UDPAgentWriteCallback()
    {
        m_pAgent = NULL;
    }

    virtual int run( bool bResult, const DataContext* pContext )
    {
        if ( NULL == m_pAgent )
        {
            ERROR_LOG(
                "In UDPAgentWriteCallback::run, m_pAgent == NULL" );
            return FAILED;
        }

        m_pAgent->writeBack( bResult );
        return SUCCESSFUL;
    }

private:
    UDPAgent* m_pAgent;
};

#endif  // UDPAGENTWRITECALLBACK_H_


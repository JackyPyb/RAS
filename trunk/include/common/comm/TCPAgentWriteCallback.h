#ifndef TCPAGENTWRITECALLBACK_H_
#define TCPAGENTWRITECALLBACK_H_

#include "common/comm/WriteCallback.h"
#include "common/comm/TCPAgent.h"
#include "common/log/log.h"

class TCPAgentWriteCallback : public WriteCallback
{
public:
    explicit TCPAgentWriteCallback( TCPAgent* pAgent )
    {
        m_pAgent = pAgent;
    }

    virtual ~TCPAgentWriteCallback()
    {
        m_pAgent = NULL;
    }

    virtual int run( bool bResult, const DataContext* pContext )
    {
        if ( NULL == m_pAgent )
        {
            ERROR_LOG(
                "In TCPAgentWriteCallback::run, m_pAgent == NULL" );
            return FAILED;
        }

        m_pAgent->writeBack( bResult );
        return SUCCESSFUL;
    }

private:
    TCPAgent* m_pAgent;
};

#endif  // TCPAGENTWRITECALLBACK_H_


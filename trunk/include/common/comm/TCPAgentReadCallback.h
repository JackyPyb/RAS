#ifndef TCPAGENTREADCALLBACK_H_
#define TCPAGENTREADCALLBACK_H_

#include "common/comm/ReadCallback.h"
#include "common/comm/TCPAgent.h"
#include "common/log/log.h"

class TCPAgentReadCallback : public ReadCallback
{
public:
    explicit TCPAgentReadCallback( TCPAgent* pAgent )
    {
        m_pAgent = pAgent;
    }

    virtual ~TCPAgentReadCallback()
    {
        m_pAgent = NULL;
    }

    virtual int run( InReq& req )
    {
        if ( NULL == m_pAgent )
        {
            ERROR_LOG(
                "In TCPAgentWriteCallback::run, m_pAgent == NULL" );
            return FAILED;
        }

        m_pAgent->readBack( req );
        return SUCCESSFUL;
    }

private:
    TCPAgent* m_pAgent;
};

#endif  // TCPAGENTREADCALLBACK_H_


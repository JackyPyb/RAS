/*
 * =========================================================================
 *
 *       Filename:  UnxDmnAgentReadCallback.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012-02-05 22:36:45
 *  Last Modified:  2012-02-05 22:36:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#ifndef UNXDMNAGENTREADCALLBACK_H_
#define UNXDMNAGENTREADCALLBACK_H_

#include "common/comm/ReadCallback.h"
#include "common/comm/UnxDmnAgent.h"
#include "common/log/log.h"

class UnxDmnAgentReadCallback : public ReadCallback
{
public:
    explicit UnxDmnAgentReadCallback(
        UnxDmnAgent* pAgent )
    {
        m_pAgent = pAgent;
    }

    virtual ~UnxDmnAgentReadCallback()
    {
        m_pAgent = NULL;
    }

    virtual int run( const char* pHeader, uint32_t headerLen,
                     char* pContent, uint32_t contentLen )
    {
        if ( NULL == m_pAgent )
        {
            ERROR_LOG(
                "In UnxDmnAgentWriteCallback::run, m_pAgent == NULL" );
            return FAILED;
        }

        return m_pAgent->readCallback(
                   pHeader, headerLen, pContent, contentLen );
    }
private:
    UnxDmnAgent* m_pAgent;
};

#endif  // UNXDMNAGENTREADCALLBACK_H_


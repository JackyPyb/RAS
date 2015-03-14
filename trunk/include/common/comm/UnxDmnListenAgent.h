/*
 * =========================================================================
 *
 *       Filename:  UnxDmnListenAgent.h
 *
 *    Description:  UnixÓòÌ×½Ó×Ö¼àÌýÌ×½Ó×Ö
 *
 *        Version:  1.0
 *        Created:  2012-02-06 00:33:59
 *  Last Modified:  2012-02-06 00:33:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#ifndef UnxDmnListenAgent_H_
#define UnxDmnListenAgent_H_

#include "common/comm/Agent.h"
#include "common/comm/AgentManager.h"
#include "common/comm/UnxDmnSocket.h"
#include "common/comm/EpollEvent.h"
#include "common/comm/UnxDmnSocketAddress.h"
#include "common/sys/Singleton.h"
#include "common/log/log.h"

template <typename ConcreteAgent, typename ConcreteProtocol>
class UnxDmnListenAgent : public Agent
{
public:
    UnxDmnListenAgent( Epoll* pEpoll )
    {
        m_epollEvent.setEpoll( pEpoll );
        m_pEpoll = pEpoll;
    }

    virtual ~UnxDmnListenAgent() {}

    virtual int init( UnxDmnSocketAddress& localAddr, int backlog );

    virtual int recvData();

    virtual int sendData();

private:
    UnxDmnSocket m_socket;
    UnxDmnSocketAddress m_localAddress;
    EpollEvent m_epollEvent;
    Epoll* m_pEpoll;
};

template <typename ConcreteAgent, typename ConcreteProtocol>
int
UnxDmnListenAgent<ConcreteAgent, ConcreteProtocol>::init(
    UnxDmnSocketAddress& localAddr,
    int backlog )
{
    m_localAddress = localAddr;

    int rt = 0;

    rt = m_socket.socket();

    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnListenAgent::init, m_socket.socket() error" );
        return FAILED;
    }

    rt = m_socket.setNonBlock();

    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnListenAgent::init, "
                   "m_socket.setNonBlock() error" );
        return FAILED;
    }

    rt = m_socket.bind( m_localAddress );

    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnListenAgent::init, "
                   "m_socket.bind() error" );
        return FAILED;
    }

    rt = m_socket.listen( backlog );

    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnListenAgent::init, "
                   "m_socket.listen() error" );
        return FAILED;
    }

    m_epollEvent.setFd( m_socket.getFd() );
    m_epollEvent.setHandlerID( getID() );
    m_epollEvent.registerREvent();

    return SUCCESSFUL;
}

template <typename ConcreteAgent, typename ConcreteProtocol>
int
UnxDmnListenAgent<ConcreteAgent, ConcreteProtocol>::recvData()
{
    UnxDmnSocketAddress oppoAddr;
    int connFd;
    connFd = m_socket.accept( &oppoAddr );

    if ( connFd < 0 )
    {
        if ( !( ( errno == EINTR ) || ( errno == EWOULDBLOCK ) ) )
        {
            ERROR_LOG( "In UnxDmnListenAgent::recvData, "
                       "m_socket.accept() error" );
            return FAILED;
        }
    }

    UnxDmnSocket connSocket( connFd );

    // set nonblock
    if ( connSocket.setNonBlock() < 0 )
    {
        ERROR_LOG( "In UnxDmnListenAgent::recvData, "
                   "connSocket.setNonBlock() error" );
        return FAILED;
    }

    ConcreteAgent* pAgent = new ConcreteAgent( m_pEpoll,
                                               new ConcreteProtocol(), connSocket, oppoAddr );
    return pAgent->init();
}

template <typename ConcreteAgent, typename ConcreteProtocol>
int
UnxDmnListenAgent<ConcreteAgent, ConcreteProtocol>::sendData()
{
    return FAILED;
}

#endif  // UnxDmnListenAgent_H_


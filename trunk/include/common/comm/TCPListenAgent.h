#ifndef _TCPLISTENAGENT_H
#define _TCPLISTENAGENT_H

#include <netinet/tcp.h>
#include <iostream>
#include <cstring>
#include <errno.h>

#include "common/comm/Error.h"
#include "common/comm/Agent.h"
#include "common/comm/AgentManager.h"
#include "common/comm/TCPSocket.h"
#include "common/comm/EpollEvent.h"
#include "common/comm/SocketAddress.h"
#include "common/log/log.h"
#include "common/sys/Singleton.h"

using namespace std;

const int LISTEN_NUM = 1024;

template <class ConcreteAgent>
class TCPListenAgent : public Agent
{
public:
    TCPListenAgent( const SocketAddress& addr ):
		m_addr(addr)
    {
		
    }
    ~TCPListenAgent() {}

    virtual int init( void );
    virtual int recvData( void );
    virtual int sendData( void );

private:
    TCPSocket m_TCPListenSocket;
    SocketAddress m_addr;
    EpollEvent m_epollEvent;
};

//initialize the TCPListenAgent
template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::init( void )
{
    if ( m_TCPListenSocket.generateSocket() < 0
            || m_TCPListenSocket.disableLinger() < 0
            || m_TCPListenSocket.disableNagle() < 0
            || m_TCPListenSocket.enableReuseaddr() < 0
            || m_TCPListenSocket.setNonblock() < 0
            || m_TCPListenSocket.bind( m_addr ) < 0
            || m_TCPListenSocket.listen( LISTEN_NUM ) < 0 )
    {
        ERROR_LOG( "TCPListenAgent::init()" );
        return FAILED;
    }

    //register the read I/O event
    m_epollEvent.setFd( m_TCPListenSocket.getFd() );
    m_epollEvent.setHandlerID( getID());
    m_epollEvent.registerREvent();
    return SUCCESSFUL;
}

//when the read event arrive, accept a connecton,
//create a ConcreteAgent and initialize it
template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::recvData( void )
{
    SocketAddress oppositeAddr;
    int connfd;
    connfd = m_TCPListenSocket.accept( oppositeAddr );
    if ( connfd < 0 )
    {
        if ( !( ( errno == EINTR ) || ( errno == EWOULDBLOCK ) ) )
        {
            ERROR_LOG( "TCPListenAgent listen" );
            return FAILED;
        }
    }
    TCPSocket connSock( connfd );
    if ( connSock.setNonblock() < 0
            || connSock.disableLinger() < 0
            || connSock.disableNagle() < 0 )
    {
        ERROR_LOG( "TCPListenAgent::recvData" );
        connSock.close();
        return FAILED;
    }
    //resigerREvent and initialize the Agent's member
    ConcreteAgent* agent =
        ( AgentManager::getInstance() )->createAgent<ConcreteAgent>(
            connSock, oppositeAddr);
    agent->init();

    return SUCCESSFUL;
}

//when the write event arrive, do nothing
template <class ConcreteAgent>
int TCPListenAgent<ConcreteAgent>::sendData( void )
{
    return FAILED;
}

#endif

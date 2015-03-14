#include <string>
#include <errno.h>

#include "common/comm/Agent.h"
#include "common/comm/BaseHeader.h"
#include "common/comm/SocketAddress.h"
#include "common/log/log.h"

Agent::Agent():
    m_nRefCount( 0 ), 
    m_bIsRecycler( false ), 
    m_iConnect( CONNECTING ),
    m_iConnectTimes( -1 ),
    m_ID(0)
{
}
//the base class which is inherited by other agents such as class DHTUdpAgent
Agent::~Agent()
{
}

int Agent::getErrno()
{
    return errno;
}

int Agent::allowReconnect()const
{
    if ( m_iConnectTimes < MAXRECONNECTTIMES )
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Agent::recvData( void )
{
    return SUCCESSFUL;
}

int Agent::sendData( void )
{
    return SUCCESSFUL;
}

int Agent::recvReq( BaseRequest* )
{
    return SUCCESSFUL;
}

int Agent::connectAfter( bool )
{
    return SUCCESSFUL;
}
int Agent::connect( const SocketAddress& )
{
    return SUCCESSFUL;
}

void Agent::recycler( void )
{
    m_bIsRecycler = true;
}

bool Agent::isRecycler( void )
{
    return m_bIsRecycler;
}

long Agent::getRefCount()const
{
    return m_nRefCount;
}

void Agent::release()
{
    --m_nRefCount;
}

int Agent::getState()const
{
    return m_iConnect;
}

void Agent::setState( int st )
{
    m_iConnect = st;
}

Agent* Agent::ref()
{
    ++m_nRefCount;
    return this;
}

void Agent::setID( uint32_t id )
{
    //INFO_LOG("Agent::setID %u", id);
    m_ID = id;
    return;
}

uint32_t Agent::getID( void )const
{
    //INFO_LOG("Agent::getID %u", m_ID);
    return m_ID;
}

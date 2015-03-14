#include <sys/types.h>

#include "common/comm/EpollEvent.h"
#include "common/comm/UDPAgent.h"
#include "common/comm/UDPReader.h"
#include "common/comm/UDPWriter.h"
#include "common/comm/UDPAgentReadCallback.h"
#include "common/comm/UDPAgentWriteCallback.h"

using namespace std;

#define PASSIVE false
#define ACTIVE true

UDPAgent::UDPAgent()
{
    m_nRefCount = 1;
    m_iLen = 0;
    m_bAgentType = PASSIVE;
}

//the parameter is the opposit end 's SocketAddress
UDPAgent::UDPAgent( const SocketAddress& oppoAddr )
    : m_oppoAddr( oppoAddr )
{
    INFO_LOG( "UDPAgent::ACTIVE" );
    m_bAgentType = ACTIVE;
    m_nRefCount = 1;
    m_iLen = 0;
}

UDPAgent::~UDPAgent( void )
{
    try
    {
        this->recycler();
    }
    catch ( ... )
    {
    }
}

void UDPAgent::recycler( void )
{
    if ( m_bIsRecycler )
    {
        return;
    }
    m_bIsRecycler = true;

    if ( m_epollEvent.unregisterRWEvents() < 0 )
    {
        INFO_LOG( "m_epollEvent.unregisterRWEvents() error" );
    }
    m_epollEvent.setFd( -1 );
    if ( m_socket.close() < 0 )
    {
        INFO_LOG( "m_socket.close() error" );
    }
}

//the parameter is the Server's SocketAddress
int UDPAgent::init( const SocketAddress& addr )
{
    if ( m_socket.generateSocket() < 0
            || m_socket.enableReuseaddr() < 0
            || m_socket.setNonblock() < 0
            || m_socket.enableBroadcast() < 0 )
    {
        ERROR_LOG( "UDPAgent:initialize error" );
        return FAILED;
    }
    if ( !m_bAgentType )
    {
        if ( m_socket.bind( addr ) < 0 )
        {
            ERROR_LOG( "UDPAgent::init():bind() error" );
            return FAILED;
        }
    }
    m_epollEvent.setFd( m_socket.getFd() );
    m_epollEvent.setHandlerID( getID() );
    m_buffer.setReader( new UDPReader( m_socket ) );
    m_buffer.setWriter( new UDPWriter( m_socket ) );
    m_buffer.setReadCallback( new UDPAgentReadCallback( this ) );
    m_buffer.setWriteCallback( new UDPAgentWriteCallback( this ) );

    if ( m_epollEvent.registerREvent() < 0 )
    {
        INFO_LOG( "m_epollEvent.registerREvent error" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int UDPAgent::recvData( void )
{
    if ( readData() < 0 )
    {
        ERROR_LOG( "UDPAgent::recvData(void):readData() error" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int UDPAgent::sendData( void )
{
    if ( writeData() < 0 )
    {
        ERROR_LOG( "UDPAgent::sendData(void):writeData()" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int UDPAgent::writeData( void )
{
    if ( m_buffer.write() < 0 )
    {
        ERROR_LOG( "UDPAgent::writeData(void):m_buffer.write() error" );
        return FAILED;
    }
    if ( this->m_buffer.getSendBufferLength() == 0 )
    {
        if ( m_epollEvent.closeWevent() < 0 )
        {
            INFO_LOG( "m_epollEvent.closeWevent error" );
        }
    }

    return SUCCESSFUL;
}

int UDPAgent::readData( void )
{
    if ( m_buffer.readUDP() < 0 )
    {
        ERROR_LOG( "UDPAgent::readData(void):m_buffer.read() error" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
UDPAgent::writeToBuffer( char* buf , unsigned int len )
{
    this->m_buffer.writeToBuffer( buf, len, m_oppoAddr );

    if ( this->m_epollEvent.openWevent() < 0 )
    {
        ERROR_LOG( "UDPAgent::writeDynData():openWevent() error" );
        return FAILED;
    }

    return SUCCESSFUL;
}

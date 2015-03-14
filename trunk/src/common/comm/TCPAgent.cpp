#include <errno.h>
#include <fstream>
#include <iostream>
#include <cstring>

#include "common/comm/Epoll.h"
#include "common/comm/TCPReader.h"
#include "common/comm/TCPWriter.h"
#include "common/comm/TCPAgentReadCallback.h"
#include "common/comm/TCPAgentWriteCallback.h"

using namespace std;

//constructor
TCPAgent::TCPAgent(): 
    m_addr( InitAddr ),
    m_isActive( true )
{
    m_iLen = 0;
    m_iConnectTimes = -1;
}

TCPAgent::TCPAgent( const SocketAddress& oppoAddr ):
    m_addr( oppoAddr ), m_iLen( 0 ), m_isActive( true )
{
    m_iConnectTimes = -1;
}

//constructor
TCPAgent::TCPAgent(
    const TCPSocket& sock,
    const SocketAddress& oppoAddr
):
    m_socket( sock ), m_addr( oppoAddr ), m_iLen( 0 ), m_isActive( false )
{
    m_iConnectTimes = -1;
    TCPAgent::setState( CONNECTED );
    m_epollEvent.setFd( m_socket.getFd() );

    m_buffer.setReader( new TCPReader( m_socket ) );
    m_buffer.setWriter( new TCPWriter( m_socket ) );
    m_buffer.setReadCallback( new TCPAgentReadCallback( this ) );
    m_buffer.setWriteCallback( new TCPAgentWriteCallback( this ) );

    if ( m_epollEvent.registerREvent() < 0 )
    {
        INFO_LOG( "m_epollEvent.registerREvent Error" );
    }
}

int TCPAgent::init()
{
    m_epollEvent.setHandlerID( getID());
    return SUCCESSFUL;
}

//destructor
TCPAgent::~TCPAgent()
{
    try
    {
        //this->recycler();
    }
    catch ( ... )
    {
    }
}

int TCPAgent::getErrno()
{
    if ( m_socket.connect( m_addr ) < 0 )
    {

    }
    return errno;
}

void TCPAgent::recycler()
{
    if ( m_bIsRecycler )
    {
        return;
    }
    m_bIsRecycler = true;
#ifdef DEBUG
    if ( false == m_isActive )
    {
        INFO_LOG( "*** Client Closed, IP is %s ***", m_addr.getIP() );
    }
#endif
    if ( m_epollEvent.unregisterRWEvents() < 0 )
    {
        INFO_LOG( "m_epollEvent.unregisterRWEvents() error" );
    }
    m_epollEvent.setFd( -1 );
    if ( this->m_socket.close() < 0 )
    {
        INFO_LOG( "m_socket.close error" );
    }
}

//intialize the TCPAgent,set the nonblock socket
int TCPAgent::connect()
{
    m_iConnectTimes += 1;
    if ( m_socket.close() < 0 )
    {
        INFO_LOG( "m_socket.close error" );
    }

    if ( m_socket.generateSocket() < 0
            || m_socket.disableNagle() < 0
            || m_socket.disableLinger() < 0
            || m_socket.setNonblock() < 0
            || m_socket.setKeepAlive() < 0 )
    {
        ERROR_LOG( "TCPAgent::connect():set the socket option error " );
        return FAILED;
    }
    m_epollEvent.setFd( m_socket.getFd() );
    m_epollEvent.setHandlerID( getID());
    m_buffer.setReader( new TCPReader( m_socket ) );
    m_buffer.setWriter( new TCPWriter( m_socket ) );
    m_buffer.setReadCallback( new TCPAgentReadCallback( this ) );
    m_buffer.setWriteCallback( new TCPAgentWriteCallback( this ) );

    if ( m_epollEvent.registerRWEvents() < 0 )
    {
        return FAILED;
    }
    if ( m_socket.connect( m_addr ) < 0 )
    {

    }

    return SUCCESSFUL;
}

int TCPAgent::sendData( void )
{
    if ( this->m_buffer.write() < 0 )
    {
        ERROR_LOG( "TCPAgent::writeData():m_buffer.write(TCPSocket)" );
        return FAILED;
    }
    if ( this->m_buffer.getSendBufferLength() == 0 )
    {
        if ( this->m_epollEvent.closeWevent() < 0 )
        {
            INFO_LOG( "this->m_epollEvent.closeWevent error" );
        }
    }

    return SUCCESSFUL;
}

int TCPAgent::recvData( void )
{
    if ( ( this->m_buffer.readTCP() ) < 0 )
    {
        ERROR_LOG( "TCPAgent::readData():m_buffer.read()" );
        ERROR_LOG( 
            "opposit: %s " , m_addr.getIP()); //pxy
        return FAILED;
    }

    return SUCCESSFUL;
}

//write dynamic messages
int TCPAgent::writeToBuffer( char* buf , unsigned int len )
{
    this->m_buffer.writeToBuffer( buf, len );

    if ( this->m_epollEvent.openWevent() < 0 )
    {
        INFO_LOG( "m_epollEvent.openWevent error" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int TCPAgent::connectAfter( bool )
{
    return SUCCESSFUL;
}

void TCPAgent::setState( int st )
{
    m_iConnect = CONNECTED;
    if ( st == CONNECTED )
    {
        if ( m_epollEvent.closeWevent() < 0 )
        {
            INFO_LOG( "m_epollEvent.closeWevent error" );
        }
    }
}


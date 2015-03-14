#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "common/comm/BaseHeader.h"
#include "common/comm/UDPSocket.h"
#include "common/comm/SocketAddress.h"
#include "common/comm/Error.h"
#include "common/log/log.h"

int UDPSocket::ipType = IPV4;

UDPSocket::UDPSocket()
    : m_sockFd( -1 )
{
}

UDPSocket::~UDPSocket()
{
}

int UDPSocket::generateSocket( void )
{
    if ( IPV4 == ipType )
    {
        m_sockFd = socket( PF_INET, SOCK_DGRAM, 0 );
    }
    else
    {
        m_sockFd = socket( PF_INET6, SOCK_DGRAM, 0 );
    }
    if ( m_sockFd < 0 )
    {
        ERROR_LOG( "UDPSocket::generateSocket::socket() error" );
    }
    return m_sockFd;
}

int UDPSocket::getFd( void )const
{
    return m_sockFd;
}

int UDPSocket::close( void )
{
    if ( m_sockFd == -1 )
    {
        return SUCCESSFUL;
    }
    if ( ::close( m_sockFd ) < 0 )
    {
        ERROR_LOG( "UDPSocket::close::close() error" );
        return FAILED;
    }
    m_sockFd = -1;
    return SUCCESSFUL;
}

int UDPSocket::bind( const SocketAddress& servaddr )
{
    if ( ipType == IPV4 )
    {
        struct sockaddr_in addr;
        memset( &addr, 0, sizeof( addr ) );
        addr.sin_family = PF_INET;

        if ( servaddr.ifAnyAddr() )
        {
            addr.sin_addr.s_addr = htonl( INADDR_ANY );
        }
        else
        {
            if ( ( inet_aton(
                        servaddr.getIP(), 
                        ( in_addr* )&addr.sin_addr.s_addr ) ) == 0 )
            {
                ERROR_LOG(
                    "UDPSocket::bind::inet_addr: IP Address Invalid" );
                return FAILED;
            }
        }

        if ( servaddr.getPort() == 0 )
        {
            ERROR_LOG( "UDPSocket::bind: Port Invalid" );
            return FAILED;
        }
        addr.sin_port = htons( servaddr.getPort() );
        if ( ::bind(
            m_sockFd, ( const struct sockaddr* )&addr, sizeof( addr ) ) < 0 )
        {
            ERROR_LOG( "UDPSocket::bind::bind() error" );
            return FAILED;
        }
    }
    else
    {
        struct sockaddr_in6 addr;
        memset( &addr, 0, sizeof( addr ) );
        addr.sin6_family = PF_INET6;

        if ( servaddr.ifAnyAddr() )
        {
            addr.sin6_addr = in6addr_any;
        }
        else
        {
            if ( inet_pton( 
                PF_INET6, servaddr.getIP(), &( addr.sin6_addr ) ) < 0 )
            {
                ERROR_LOG(
                    "UDPSocket::bind::inet_addr: IP Address Invalid" );
                return FAILED;
            }
        }
        if ( servaddr.getPort() == 0 )
        {
            ERROR_LOG( "UDPSocket::bind: Port Invalid" );
            return FAILED;
        }
        addr.sin6_port = htons( servaddr.getPort() );
        addr.sin6_scope_id = 2;
        if ( ::bind( 
            m_sockFd, ( const struct sockaddr* )&addr, sizeof( addr ) ) < 0 )
        {
            ERROR_LOG( "UDPSocket::bind::bind() error" );
            return FAILED;
        }
    }

    return SUCCESSFUL;

}

int
UDPSocket::enableBroadcast( void )
{
    int val = 1;
    if ( setsockopt(
        m_sockFd, 
        SOL_SOCKET, 
        SO_BROADCAST, 
        ( const void* )&val, sizeof( val ) ) < 0 )
    {
        ERROR_LOG( "UDPSocket::enableBroadcast::setsockopt() error" );
        return FAILED;
    }
    return SUCCESSFUL;
}

int
UDPSocket::enableReuseaddr( void )
{
    int val = 1;
    if ( setsockopt(
        m_sockFd, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        ( const void* )&val, sizeof( val ) ) < 0 )
    {
        ERROR_LOG( "UDPSocket::enableReuseaddr::setsockopt() error" );
        return FAILED;
    }
    return SUCCESSFUL;
}

int
UDPSocket::setNonblock( void )
{
    int val;
    if ( ( val = fcntl( m_sockFd, F_GETFL, 0 ) ) < 0 )
    {
        ERROR_LOG( "UDPSocket::setNonBlock::fcntl() error" );
        return val;
    }
    val |= O_NONBLOCK;
    if ( fcntl( m_sockFd, F_SETFL, val ) < 0 )
    {
        ERROR_LOG( "UDPSocket::setNonBlock:fcntl() error" );
        return FAILED;
    }
    return SUCCESSFUL;
}


int
UDPSocket::read( char* buf, size_t len, struct sockaddr_in& cliaddr )
{
    socklen_t socklen = sizeof( cliaddr );

    int readNum = ::recvfrom( m_sockFd, buf, len, 0,
                              ( struct sockaddr* )&cliaddr,
                              ( socklen_t* )&socklen );

    return readNum;
}

int
UDPSocket::write( 
    const char* buf, size_t len, const struct sockaddr_in& cliaddr )
{
    int writeNum = ::sendto( m_sockFd, buf, len, 0,
                             ( const struct sockaddr* )&cliaddr,
                             ( socklen_t )( sizeof( cliaddr ) ) );

    return writeNum;
}


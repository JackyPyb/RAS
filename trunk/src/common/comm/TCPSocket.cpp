#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#include "common/comm/SocketAddress.h"
#include "common/comm/TCPSocket.h"
#include "common/comm/Error.h"
#include "common/comm/BaseHeader.h"
#include "common/comm/Tool.h"
#include "common/log/log.h"

int TCPSocket::ipType = IPV4;

TCPSocket::TCPSocket()
    : m_sockFd( -1 )
{
}

TCPSocket::TCPSocket( int fd )
    : m_sockFd( fd )
{
}

TCPSocket::~TCPSocket()
{
}

int
TCPSocket::generateSocket( void )
{
    if ( ipType == IPV4 )
    {
        m_sockFd = socket( PF_INET, SOCK_STREAM, 0 );
    }
    else
    {
        m_sockFd = socket( PF_INET6, SOCK_STREAM, 0 );
    }
    if ( m_sockFd < 0 )
    {
        ERROR_LOG( "TCPSocket::TCPSocket::socket" );
    }

    return m_sockFd;
}

int
TCPSocket::getFd( void )const
{
    return m_sockFd;
}

int
TCPSocket::close( void )
{
    if ( m_sockFd == -1 )
    {
        return SUCCESSFUL;
    }
    if ( ::close( m_sockFd ) < 0 )
    {
        ERROR_LOG( "TCPSocket::close::close" );
        return FAILED;
    }
    m_sockFd = -1;

    return SUCCESSFUL;
}

int
TCPSocket::read( char* buf, size_t len )
{
    int readNum = ::read( m_sockFd, buf, len );
    return readNum;
}

int
TCPSocket::write( const char* buf, size_t len )
{
    int writeNum = ::write( m_sockFd, buf, len );
    return writeNum;
}

int
TCPSocket::disableLinger( void )
{
    struct linger ling = {0, 0};
    if ( setsockopt( 
        m_sockFd, SOL_SOCKET, SO_LINGER, &ling, sizeof( ling ) ) < 0 )
    {
        ERROR_LOG( "TCPSocket::disableLinger::setsockopt" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
TCPSocket::enableReuseaddr( void )
{
    int val = 1;
    if ( setsockopt( 
        m_sockFd, 
        SOL_SOCKET, 
        SO_REUSEADDR, 
        ( const void* )&val, sizeof( val ) ) < 0 )
    {
        ERROR_LOG( "TCPSocket::enableReuseaddr::setsockopt" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
TCPSocket::disableNagle( void )
{
    int val = 1;
    if ( setsockopt( 
        m_sockFd, 
        IPPROTO_TCP, 
        TCP_NODELAY, 
        ( const void* )&val, sizeof( val ) ) < 0 )
    {
        ERROR_LOG( "TCPSocket::disableNagle::setsockopt" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
TCPSocket::setNonblock( void )
{
    int val;

    if ( ( val = fcntl( m_sockFd, F_GETFL, 0 ) ) < 0 )
    {
        ERROR_LOG( "TCPSocket::setNonBlock::fcntl-F_GETFL" );
        return val;
    }
    val |= O_NONBLOCK;
    if ( fcntl( m_sockFd, F_SETFL, val ) < 0 )
    {
        ERROR_LOG( "TCPSocket::setNonBlock:fcntl" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
TCPSocket::bind( const SocketAddress& servaddr )
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
                servaddr.getIP(), ( in_addr* )&addr.sin_addr.s_addr ) ) == 0 )
            {
                ERROR_LOG(
                    "TCPSocket::bind::inet_addr: IP Address Invalid" );
                return FAILED;
            }
        }

        if ( servaddr.getPort() == 0 )
        {
            ERROR_LOG( "TCPSocket::bind: Port Invalid" );
            return FAILED;
        }
        addr.sin_port = htons( servaddr.getPort() );
        if ( ::bind(
            m_sockFd, ( const struct sockaddr* )&addr, sizeof( addr ) ) < 0 )
        {
            ERROR_LOG(
                "TCPSocket::bind : bind error, Port is %u" ,
                servaddr.getPort() );
            return FAILED;
        }
    }
    else
    {
        struct sockaddr_in6 addr;
        memset( &addr, 0, sizeof( addr ) );
        addr.sin6_family = PF_INET6;
        addr.sin6_scope_id = 2;
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
                    "TCPSocket::bind::inet_addr: IP Address Invalid" );
                return FAILED;

            }
        }

        if ( servaddr.getPort() == 0 )
        {
            ERROR_LOG( "TCPSocket::bind: Port Invalid" );
            return FAILED;
        }
        addr.sin6_port = htons( servaddr.getPort() );
        if ( ::bind( 
            m_sockFd, ( const struct sockaddr* )&addr, sizeof( addr ) ) < 0 )
        {
            ERROR_LOG( "TCPSocket::bind::bind" );
            return FAILED;
        }

    }

    return SUCCESSFUL;
}

int
TCPSocket::listen( int qs )
{
    if ( ::listen( m_sockFd, qs ) < 0 )
    {
        ERROR_LOG( "TCPSocket::listen::listen" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
TCPSocket::accept( SocketAddress& addr )
{
    if ( IPV4 == ipType )
    {
        struct sockaddr_in cliAddr;
        unsigned int cliAddrLen = sizeof( cliAddr );
        memset( &cliAddr, 0, cliAddrLen );
        int fd = ::accept(
            m_sockFd, ( struct sockaddr* )&cliAddr, &cliAddrLen );
        addr.setAddress(
            inet_ntoa( cliAddr.sin_addr ), ntohs( cliAddr.sin_port ) );
        return fd;
    }
    else
    {
        struct sockaddr_in6 cliAddr;
        unsigned int cliAddrLen = sizeof( cliAddr );
        memset( &cliAddr, 0, cliAddrLen );
        int fd = ::accept( 
            m_sockFd, ( struct sockaddr* )&cliAddr, &cliAddrLen );
        char str[46];
        addr.setAddress(
            inet_ntop( AF_INET6, &cliAddr.sin6_addr, str, sizeof( str ) ),
            ntohs( cliAddr.sin6_port ) );
        return fd;
    }
}

int
TCPSocket::connect( const SocketAddress&  ipaddr )
{
    int ret = -1;
    if ( ipType == IPV4 )
    {
        struct sockaddr_in addr;
        memset( &addr, 0, sizeof( addr ) );
        addr.sin_family = PF_INET;
        if ( ipaddr.ifAnyAddr() )
        {
            ERROR_LOG( "TCPSocket::connect:ipaddr.ip NULL" );
            return FAILED;
        }
        else
        {
            if ( ( addr.sin_addr.s_addr = inet_addr( ipaddr.getIP() ) ) 
                == INADDR_NONE )
            {
                ERROR_LOG(
                    "TCPSocket::bind::inet_addr: IP Address Invalid" );
                return FAILED;
            }
        }
        if ( ipaddr.getPort() == 0 )
        {
            ERROR_LOG( "TCPSocket::connect:ipaddr.Port Invalid" );
            return FAILED;
        }
        addr.sin_port = htons( ipaddr.getPort() );

        ret = ::connect( m_sockFd, ( struct sockaddr* )&addr, sizeof( addr ) );

    }
    else
    {
        struct sockaddr_in6 addr;
        memset( &addr, 0, sizeof( addr ) );
        addr.sin6_family = PF_INET6;
        if ( ipaddr.ifAnyAddr() )
        {
            ERROR_LOG( "TCPSocket::connect:ipaddr.ip AnyAddr" );
            return FAILED;
        }
        else
        {
            if ( inet_pton(
                PF_INET6, ipaddr.getIP(), &( addr.sin6_addr ) ) < 0 )
            {
                ERROR_LOG(
                    "TCPSocket::bind::inet_addr: IP Address Invalid" );
                return FAILED;
            }
        }
        if ( ipaddr.getPort() == 0 )
        {
            ERROR_LOG( "TCPSocket::connect:ipaddr.Port Invalid" );
            return FAILED;
        }
        addr.sin6_port = htons( ipaddr.getPort() );
        addr.sin6_scope_id = 2;
        ret = ::connect( m_sockFd, ( struct sockaddr* )&addr, sizeof( addr ) );
        if ( ret < 0 )
        {
            ERROR_LOG( "TCPSocket::connecSocket" );
        }
    }

    return ret;

}

int
TCPSocket::writev( const struct iovec* v, size_t c )
{
    int writeNum = ::writev( m_sockFd, v, c );
    return writeNum;
}

int
TCPSocket:: readn( char* vptr, unsigned int n )
{
    unsigned int  nleft;
    int  nread;
    char* ptr;

    if ( !vptr )
    {
        return FAILED;
    }
    ptr = vptr;
    nleft = n;

    while ( nleft > 0 )
    {
        if ( ( nread = ::read( m_sockFd, ptr, nleft ) ) < 0 )
        {
            if ( errno == EINTR )
            {
                nread = 0;
            }
            else if ( errno != EWOULDBLOCK )
            {
                ERROR_LOG( "readn():" );
                return FAILED;
            }
        }
        else if ( nread == 0 )
        {
            break;
        }
        else
        {
            nleft -= ( unsigned int )nread;
            ptr += nread;
        }
    }

    return ( int )( n - nleft );
}

int
TCPSocket::setKeepAlive( void )
{
    int val = 1;
    if ( setsockopt( m_sockFd, SOL_SOCKET, SO_KEEPALIVE,
                     ( const void* )&val, sizeof( val ) ) < 0 )
    {
        ERROR_LOG( "TCPSocket::setKeepAlive::setsockopt" );
        return FAILED;
    }

    return SUCCESSFUL;
}

//for debug
int TCPSocket::Peek( char* buf, size_t size )
{
    int readNum = ::recv( m_sockFd, buf, size, MSG_PEEK );
    return readNum;
}

//for degug end
void TCPSocket::ThrowData( unsigned int size )
{
    char arr[size];
    if ( ::read( m_sockFd, arr, size ) < 0 )
    {
        return;
    }
}

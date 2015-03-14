/*
 * =========================================================================
 *
 *       Filename:  UnxDmnSocket.cpp
 *
 *    Description:  UnixÌ×½Ó×Ö·â×°
 *
 *        Version:  1.0
 *        Created:  2012-01-12 22:57:50
 *  Last Modified:  2012-02-05 18:35:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#include "common/comm/UnxDmnSocket.h"
#include "common/log/log.h"
#include "common/comm/Error.h"

#include <cstdlib>
#include <fcntl.h>
#include <sys/uio.h>

int
UnxDmnSocket::socket()
{
    int rt = ::socket( AF_LOCAL, SOCK_STREAM, 0 );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::socket, socket error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    m_sockFd = rt;

    return SUCCESSFUL;
}

int
UnxDmnSocket::bind( const UnxDmnSocketAddress& addr )
{
    m_localAddress = addr;

    // failed if path not existed, ignore
    unlink( addr.m_cstrPath.c_str() );

    struct sockaddr_un localAddr = addr.m_cSockAddr;

    int rt = ::bind( m_sockFd,
                 reinterpret_cast<struct sockaddr*>( &localAddr ),
                 SUN_LEN( &localAddr ) );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::bind, bind error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
UnxDmnSocket::listen( int backlog )
{
    int rt = ::listen( m_sockFd, backlog );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::listen, listen error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
UnxDmnSocket::connect( const UnxDmnSocketAddress& addr )
{
    m_oppoAddress = addr;

    struct sockaddr_un oppoAddr = addr.m_cSockAddr;

    int rt = ::connect( m_sockFd,
                    reinterpret_cast<struct sockaddr*>( &oppoAddr ),
                    SUN_LEN( &oppoAddr ) );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::connect, connect error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
UnxDmnSocket::accept( UnxDmnSocketAddress* pOppoAddr )
{
    struct sockaddr_un oppoAddr;
    socklen_t len = sizeof( oppoAddr );
    memset( &oppoAddr, 0, len );

    int rt = ::accept( m_sockFd,
                   reinterpret_cast<struct sockaddr*>( &oppoAddr ),
                   &len );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::accept, accept error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    if ( NULL != pOppoAddr )
    {
        pOppoAddr->setAddress( oppoAddr );
    }

    return rt;
}

int
UnxDmnSocket::close()
{
    int rt = ::close( m_sockFd );
    m_sockFd = -1;
    if ( rt < 0 )
    {
        WARN_LOG( "In UnxDmnSocket::close, close error: %s",
                  strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
UnxDmnSocket::setNonBlock()
{
    int val;
    if ( ( val = ::fcntl( m_sockFd, F_GETFL, 0 ) ) < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::setNonBlock, fcntl(F_GETFL) error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    val |= O_NONBLOCK;

    if ( ::fcntl( m_sockFd, F_SETFL, val ) < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::setNonBlock, fcntl(F_SETFL) error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
UnxDmnSocket::read( char* pBuf, size_t len )
{
    if ( NULL == pBuf )
    {
        FATAL_LOG( "In UnxDmnSocket::read, pBuf == NULL" );
        return FAILED;
    }

    int rt = ::read( m_sockFd, pBuf, len );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::read, read error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return rt;
}

int
UnxDmnSocket::readn( char* pBuf, size_t len )
{
    if ( NULL == pBuf )
    {
        FATAL_LOG( "In UnxDmnSocket::readn, pBuf == NULL" );
        return FAILED;
    }

    size_t nleft;
    ssize_t nread;
    char* ptr = pBuf;

    nleft = len;
    nread = 0;

    while ( nleft > 0 )
    {
        if ( ( nread = ::read( m_sockFd, pBuf, nleft ) ) < 0 )
        {
            // may error
            if ( errno == EINTR )
            {
                // interrupt
                nread = 0;
                continue;
            }
            else
            {
                // error
                ERROR_LOG( "In UnxDmnSocket::readn, read error: %s",
                           strerror( errno ) );
                return FAILED;
            }
        }
        else if ( nread == 0 )
        {
            // EOF
            break;
        }

        // some data read
        nleft -= nread;
        ptr += nread;
    }

    return ( len - nleft );
}

int
UnxDmnSocket::write( const char* pBuf, size_t len )
{
    if ( NULL == pBuf )
    {
        FATAL_LOG( "In UnxDmnSocket::write, pBuf == NULL" );
        return FAILED;
    }

    int rt = ::write( m_sockFd, pBuf, len );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::write, write error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return rt;
}

int
UnxDmnSocket::writen( const char* pBuf, size_t len )
{
    if ( NULL == pBuf )
    {
        FATAL_LOG( "In UnxDmnSocket::writen, pBuf == NULL" );
        return FAILED;
    }

    size_t nleft;
    ssize_t nwritten;
    const char* ptr = pBuf;

    nleft = len;
    nwritten = 0;

    while ( nleft > 0 )
    {
        if ( ( nwritten = ::write( m_sockFd, pBuf, nleft ) ) < 0 )
        {
            // may error
            if ( errno == EINTR )
            {
                // interrupt
                nwritten = 0;
                continue;
            }
            else
            {
                // error
                ERROR_LOG( "In UnxDmnSocket::writen, write error: %s",
                           strerror( errno ) );
                return FAILED;
            }
        }
        else if ( nwritten == 0 )
        {
            break;
        }

        // some data written
        nleft -= nwritten;
        ptr += nwritten;
    }

    return ( len - nleft );

}

int
UnxDmnSocket::writev( const struct iovec* pIov, size_t iovCnt )
{
    if ( NULL == pIov )
    {
        FATAL_LOG( "In UnxDmnSocket::writev, pIov == NULL" );
        return FAILED;
    }

    int rt = ::writev( m_sockFd, pIov, iovCnt );
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnSocket::writev, writev error: %s",
                   strerror( errno ) );
        return FAILED;
    }

    return rt;
}


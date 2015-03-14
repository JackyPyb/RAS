/*
 * =====================================================================================
 *
 *       Filename:  io.cpp
 *
 *    Description:  IO操作工具
 *
 *        Version:  1.0
 *        Created:  2011-04-29 16:04:03
 *  Last Modified:  2011-04-29 16:04:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include <cerrno>
#include <cstddef>
#include <unistd.h>
#include <fcntl.h>

#include "common/log/log.h"
#include "common/comm/Error.h"
#include "common/util/io.h"

ssize_t util::io::readn( int fd, void* vptr, size_t n )
{
    if ( NULL == vptr )
    {
        DEBUG_LOG( "NULL pointer." );
        return FAILED;
    }

    size_t nleft;
    ssize_t nread;
    char* ptr = static_cast<char*>( vptr );

    nleft = n;
    nread = 0;

    while ( nleft > 0 )
    {
        if ( ( nread = read( fd, ptr, nleft ) ) < 0 )
        {
            if ( errno == EINTR )
            {
                nread = 0;
                continue;
            }
            else
            {
                DEBUG_LOG( "Syscall Error: read. %s.", strerror( errno ) );
                return FAILED;
            }
        }
        else if ( nread == 0 )
        {
            break;  // EOF
        }

        nleft -= nread;
        ptr += nread;
    }

    return ( n - nleft );
}

ssize_t util::io::writen( int fd, const void* vptr, size_t n )
{
    if ( NULL == vptr )
    {
        DEBUG_LOG( "NULL pointer." );
        return FAILED;
    }

    size_t nleft;
    ssize_t nwritten;
    char* ptr = static_cast<char*>( const_cast<void*>( vptr ) );

    nleft = n;
    nwritten = 0;

    while ( nleft > 0 )
    {
        if ( ( nwritten = write( fd, ptr, nleft ) ) < 0 )
        {
            if ( errno == EINTR )
            {
                nwritten = 0;
                continue;
            }
            else
            {
                DEBUG_LOG( "Syscall Error: write. %s", strerror( errno ) );
                return FAILED;
            }
        }
        else if ( nwritten == 0 )
        {
            break;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return ( n - nleft );
}

int
util::io::setNonblock( int fd )
{
    int val;

    if ( ( val = fcntl( fd, F_GETFL, 0 ) ) < 0 )
    {
        DEBUG_LOG( "Syscall Error: fcntl. %s.", strerror( errno ) );
        return val;
    }

    val |= O_NONBLOCK;
    if ( fcntl( fd, F_SETFL, val ) < 0 )
    {
        DEBUG_LOG( "Syscall Error: fcntl. %s.", strerror( errno ) );
        return FAILED;
    }

    return SUCCESSFUL;
}


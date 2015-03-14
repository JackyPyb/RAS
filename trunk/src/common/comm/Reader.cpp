/*
 * =========================================================================
 *
 *       Filename:  Reader.cpp
 *
 *    Description:  数据接收器
 *
 *        Version:  1.0
 *        Created:  2012-01-16 01:24:59
 *  Last Modified:  2012-01-16 01:24:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#include "common/comm/Reader.h"
#include "common/log/log.h"

int
Reader::doRead( char* pBuf, size_t len )
{
    int rt = doReadInternal( pBuf, len );
    if ( rt < 0 )
    {
        ERROR_LOG( "In Reader::doRead, Reader::doReadInternal error" );
    }
    else if ( 0 == rt )
    {
        DEBUG_LOG( "In Reader::doRead, EOF encountered" );
    }

    return rt;
}

int
Reader::doRead( char* pBuf, size_t len, struct sockaddr_in& addr )
{
    int rt = doReadInternal( pBuf, len, addr );
    if ( rt < 0 )
    {
        ERROR_LOG( "In Reader::doRead, Reader::doReadInternal error" );
    }
    else if ( 0 == rt )
    {
        DEBUG_LOG( "In Reader::doRead, EOF encountered" );
    }

    return rt;
}


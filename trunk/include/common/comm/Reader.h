/*
 * =========================================================================
 *
 *       Filename:  Reader.h
 *
 *    Description:  数据接收器
 *
 *        Version:  1.0
 *        Created:  2012-01-16 01:19:51
 *  Last Modified:  2012-01-16 01:19:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#ifndef READER_H_
#define READER_H_

#include "common/comm/Error.h"

#include <stdio.h>
#include <sys/types.h>

/**
 * @brief 数据接收器
 */
class Reader
{
public:
    Reader()
    {

    }

    virtual ~Reader()
    {

    }

    /**
     * @brief
     *
     * @param pBuf 读缓冲
     * @param len 最大读取字节数
     *
     * @return 实际读取字节数
     */
    virtual int doRead( char* pBuf, size_t len );

    virtual int doRead(
        char* pBuf, size_t len, struct sockaddr_in& addr );

protected:
    /**
     * @brief
     *
     * @param pBuf
     * @param len
     *
     * @return 实际读取字节数
     */
    virtual int doReadInternal( char* pBuf, size_t len )
    {
        if ( pBuf != NULL )
        {
            delete pBuf;
            pBuf = NULL;
        }
        return SUCCESSFUL;
    }

    virtual int doReadInternal(
        char* pBuf, size_t len, struct sockaddr_in& addr )
    {
        if ( pBuf != NULL )
        {
            delete pBuf;
            pBuf = NULL;
        }
        return SUCCESSFUL;
    }

protected:
    // can't copy
    Reader( const Reader& reader );
    Reader& operator=( const Reader& reader );
};

#endif  // READER_H_


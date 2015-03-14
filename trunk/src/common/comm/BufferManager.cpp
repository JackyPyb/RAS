/*
 * =========================================================================
 *
 *       Filename:  BufferManager.cpp
 *
 *    Description:  数据收发缓冲管理
 *
 *        Version:  1.0
 *        Created:  2012-01-16 18:32:11
 *  Last Modified:  2012-01-16 18:32:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#include "common/comm/BufferManager.h"
#include "common/log/log.h"

#include <strings.h>

const uint32_t UDPTEMPBUFLEN = 1024;

BufferManager::BufferManager()
{
    m_pReader = NULL;
    m_pWriter = NULL;
    m_pReadCallback = NULL;
    m_pWriteCallback = NULL;
    m_pAppProtoSpec = NULL;

    m_nReadOffset = 0;
    m_nHeaderSize = 0;
    m_nContentLength = 0;
    m_bReadHeader = true;
    m_bNewPackage = true;

    m_pHeader = NULL;
    m_pContent = NULL;

    bzero((char*)&m_inReq, sizeof(m_inReq));
}

BufferManager::BufferManager( 
    Reader* pReader, Writer* pWriter,
    ReadCallback* pReadCallback, WriteCallback* pWriteCallback,
    AppProtoSpec* pAppProtoSpec )
{
    m_pReader = pReader;
    m_pWriter = pWriter;
    m_pReadCallback = pReadCallback;
    m_pWriteCallback = pWriteCallback;
    m_pAppProtoSpec = pAppProtoSpec;

    m_nReadOffset = 0;
    m_nHeaderSize = 0;
    m_nContentLength = 0;
    m_bReadHeader = true;
    m_bNewPackage = true;

    m_pHeader = NULL;
    m_pContent = NULL;
}

BufferManager::~BufferManager()
{
    try
    {
        releaseSendBuffer();

        if ( m_pHeader != NULL )
        {
            delete [] m_pHeader;
            m_pHeader = NULL;
        }
        if ( m_pContent != NULL )
        {
            delete [] m_pContent;        
            m_pContent = NULL;
        }
        if ( m_pReader != NULL )
        {
            delete m_pReader;        
            m_pReader = NULL;
        }
        if ( m_pWriter != NULL )
        {
            delete m_pWriter;        
            m_pWriter = NULL;
        }
        if ( m_pReadCallback != NULL )
        {
            delete m_pReadCallback;        
            m_pReadCallback = NULL;
        }
        if ( m_pWriteCallback != NULL )
        {
            delete m_pWriteCallback;        
            m_pWriteCallback = NULL;
        }
        if ( m_pAppProtoSpec != NULL )
        {
            delete m_pAppProtoSpec;        
            m_pAppProtoSpec = NULL;
        }
        if ( m_inReq.ioBuf != NULL )
        {
            delete []m_inReq.ioBuf;        
            m_inReq.ioBuf = NULL;
        }
    }
    catch ( ... )
    {
    }
}

void
BufferManager::releaseSendBuffer()
{
    for ( list<Iov>::iterator it = m_SendIovList.begin();
            it != m_SendIovList.end(); )
    {
        list<Iov>::iterator tempit = it;
        ++it;
        if ( tempit->m_pCompleteBuffer != NULL)
        {
            delete [] tempit->m_pCompleteBuffer;
            tempit->m_pCompleteBuffer = NULL;
        }
        if ( tempit->m_pContext != NULL )
        {
            delete tempit->m_pContext;
            tempit->m_pContext = NULL;
        }

        list<Iov>::iterator erit = m_SendIovList.erase( tempit );
        if ( erit == m_SendIovList.end() )
        {
        }
    }
}

void
BufferManager::writeToBuffer( 
    char* pBuf, size_t len, DataContext* pContext )
{
    m_SendIovList.push_back( Iov( pBuf, len, pContext ) );
}

void
BufferManager::writeToBuffer(
    char* pBuf, size_t len, SocketAddress& addr, DataContext* pContext )
{
    m_SendIovList.push_back( Iov( pBuf, len, addr, pContext ) );
}

int
BufferManager::write()
{
    if ( NULL == m_pWriter )
    {
        ERROR_LOG( "In BufferManager::write, m_pWriter == NULL" );
        return FAILED;
    }

    // 发送数据
    int rt = m_pWriter->doWrite( &m_SendIovList );

    if ( rt < 0 )
    {
        ERROR_LOG( 
            "In BufferManager::write, m_pWriter->doWrite error" );
        return FAILED;
    }

    // 回调

    // 将已发送的Iov拷贝到新list中，然后再依次回调。
    // 原因是在回调中可能以delete this的方式释放Agent，
    // 这样当前Buffer也已释放，不能再访问成员m_SendIovList
    list<Iov> cbIovList;

    for ( list<Iov>::iterator it = m_SendIovList.begin();
            it != m_SendIovList.end(); )
    {
        if ( 0 == it->m_Iovec.iov_len )
        {
            // 已发送完成
            cbIovList.push_back( *it );
            list<Iov>::iterator erit = m_SendIovList.erase( it++ );
            if ( erit == m_SendIovList.end() )
            {
            }
        }
        else
        {
            // 第一个未完成缓冲区
            break;
        }
    }

    // 依次回调
    for ( list<Iov>::iterator it = cbIovList.begin();
            it != cbIovList.end(); )
    {
        if ( NULL == m_pWriteCallback )
        {
            WARN_LOG( 
                "In BufferManager::write, m_pWriteCallback == NULL" );

        }
        else
        {
            rt = m_pWriteCallback->run( true, it->m_pContext );

            if ( rt < 0 )
            {
                ERROR_LOG( 
                    "In BufferManager::write, m_pWriteCallback->run error" );
                return FAILED;
            }
        }

        delete [] it->m_pCompleteBuffer;
        if ( it->m_pContext != NULL )
        {
            delete it->m_pContext;
            it->m_pContext = NULL;
        }
    
        list<Iov>::iterator erit = cbIovList.erase( it++ );
        if ( erit == cbIovList.end() )
        {
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readUnxDmn()
{
    if ( NULL == m_pReader )
    {
        ERROR_LOG( "In BufferManager::readUnxDmn, m_pReader == NULL" );
        return FAILED;
    }

    if ( NULL == m_pAppProtoSpec )
    {
        ERROR_LOG( 
            "In BufferManager::readUnxDmn, m_pAppProtoSpec == NULL" );
        return FAILED;
    }

    if ( m_bNewPackage )
    {
        // 开始接收新的数据包
        m_bNewPackage = false;
        m_bReadHeader = true;
        
        if ( m_pHeader != NULL )
        {
            delete [] m_pHeader;
            m_pHeader = NULL;
        }
        if ( m_pContent != NULL )
        {
            delete [] m_pContent;
            m_pContent = NULL;
        }

        m_nReadOffset = 0;
        m_nContentLength = 0;

        m_nHeaderSize = m_pAppProtoSpec->getHeaderSize();
        m_pHeader = new char[m_nHeaderSize];
    }

    int rt = 0;

    if ( m_bReadHeader )
    {
        // 读头
        rt =  readUnxDmnHeader();

        if ( rt < 0 )
        {
            return rt;
        }
    }

    if ( !m_bNewPackage && !m_bReadHeader )
    {
        // 读负载
        rt = readUnxDmnContent();

        if ( rt < 0 )
        {
            return rt;
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readUnxDmnHeader()
{
    int rt = m_pReader->doRead( m_pHeader + m_nReadOffset,
                            m_nHeaderSize - m_nReadOffset );

    if ( rt < 0 && ( errno == EWOULDBLOCK || errno == EINTR ) )
    {
        // 读阻塞或系统调用被中断
        return SUCCESSFUL;
    }
    else if ( rt < 0 )
    {
        // error
        ERROR_LOG( 
            "In BufferManager::readUnxDmnHeader, m_pReader->doRead error" );
        return FAILED;
    }
    else if ( 0 == rt )
    {
        // connection closed by opposition
        WARN_LOG( 
            "In BufferManager::readUnxDmnHeader, connection closed by opposition" );
        return FAILED;
    }

    m_nReadOffset += ( uint32_t )rt;

    if ( m_nReadOffset == m_nHeaderSize )
    {
        // 读头结束
        m_nReadOffset = 0;
        m_bReadHeader = false;

        // 取得负载数据长度
        m_nContentLength = m_pAppProtoSpec->getContentLength(
                               m_pHeader, m_nHeaderSize );

        if ( 0 == m_nContentLength )
        {
            // 无负载
            m_bNewPackage = true;

            if ( NULL == m_pReadCallback )
            {
                WARN_LOG(
                    "In BufferManager::readUnxDmnHeader, m_pReadCallback == NULL" );
                return SUCCESSFUL;
            }

            // callback
            rt = m_pReadCallback->run( m_pHeader, m_nHeaderSize,
                                       NULL, 0 );

            if ( rt < 0 )
            {
                ERROR_LOG(
                    "In BufferManager::readUnxDmnHeader, m_pReadCallback->run error" );
                return FAILED;
            }
        }
        else if ( m_nContentLength > 0 )
        {
            // 有负载
            m_pContent = new char[m_nContentLength];
            memset( m_pContent, 0 , m_nContentLength );
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readUnxDmnContent()
{
    int rt = m_pReader->doRead( m_pContent + m_nReadOffset,
                            m_nContentLength - m_nReadOffset );

    if ( rt < 0 && ( errno == EWOULDBLOCK || errno == EINTR ) )
    {
        // 读阻塞或系统调用被中断
        return SUCCESSFUL;

    }
    else if ( rt < 0 )
    {
        // error
        ERROR_LOG( 
            "In BufferManager::readUnxDmnContent, m_pReader->doRead error" );
        return FAILED;

    }
    else if ( 0 == rt )
    {
        // connection closed by opposition
        WARN_LOG(
            "In BufferManager::readUnxDmnContent, connection closed by opposition" );
        return FAILED;
    }

    m_nReadOffset += ( uint32_t )rt;

    if ( m_nContentLength == m_nReadOffset )
    {
        // 读负载结束
        m_nReadOffset = 0;
        m_bNewPackage = true;

        // callback
        if ( NULL == m_pReadCallback )
        {
            WARN_LOG(
                "In BufferManager::readUnxDmnContent, m_pReadCallback == NULL" );
            if ( m_pContent != NULL )
            {
                delete []m_pContent;
                m_pContent = NULL;
            }
            return SUCCESSFUL;
        }

        rt = m_pReadCallback->run( m_pHeader, m_nHeaderSize,
                                   m_pContent, m_nContentLength );
        if ( m_pContent != NULL )
        {
            delete []m_pContent;
            m_pContent = NULL;
        }
        if ( rt < 0 )
        {
            ERROR_LOG(
                "In BufferManager::readUnxDmnContent, m_pReadCallback->run error" );
            return FAILED;
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readTCP()
{
    if ( NULL == m_pReader )
    {
        ERROR_LOG( "In BufferManager::readTCP, m_pReader == NULL" );
        return FAILED;
    }

    if ( m_bNewPackage )
    {
        // 开始接收新的数据包
        m_bNewPackage = false;
        m_bReadHeader = true;
        
        m_nReadOffset = 0;
        m_nContentLength = 0;
        
        m_nHeaderSize = HEADER_SIZE;
        m_inReq.ioBuf = NULL;
        memset( &m_inReq.m_msgHeader, 0 , m_nHeaderSize );
    }

    int rt = 0;

    if ( m_bReadHeader )
    {
        // 读头
        rt =  readTCPHeader();

        if ( rt < 0 )
        {
            return rt;
        }
    }

    if ( !m_bNewPackage && !m_bReadHeader )
    {
        // 读负载
        rt = readTCPContent();

        if ( rt < 0 )
        {
            return rt;
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readTCPHeader()
{
    int rt = m_pReader->doRead(
             ( ( char* )( &( m_inReq.m_msgHeader ) ) ) + m_nReadOffset,
             m_nHeaderSize - m_nReadOffset );

    if ( rt < 0 && ( errno == EWOULDBLOCK || errno == EINTR ) )
    {
        // 读阻塞或系统调用被中断
        return SUCCESSFUL;
    }
    else if ( rt < 0 )
    {
        // error
        ERROR_LOG( 
            "In BufferManager::readTCPHeader, m_pReader->doRead error" );
        return FAILED;
    }
    else if ( 0 == rt )
    {
        // connection closed by opposition
        WARN_LOG( 
            "In BufferManager::readTCPHeader, connection closed by opposition" );
        return FAILED;
    }

    m_nReadOffset += ( uint32_t )rt;

    if ( m_nReadOffset == m_nHeaderSize )
    {
        // 读头结束
        m_nReadOffset = 0;
        m_bReadHeader = false;

        // 取得负载数据长度
        m_nContentLength = m_inReq.m_msgHeader.length;

        if ( 0 == m_nContentLength )
        {
            // 无负载
            m_bNewPackage = true;

            if ( NULL == m_pReadCallback )
            {
                WARN_LOG(
                    "In BufferManager::readTCPHeader, m_pReadCallback == NULL" );
                return SUCCESSFUL;
            }

            // callback
            rt = m_pReadCallback->run( m_inReq );

            if ( rt < 0 )
            {
                ERROR_LOG(
                    "In BufferManager::readTCPHeader, m_pReadCallback->run error" );
                return FAILED;
            }
        }
        else if ( m_nContentLength > 0 )
        {
            // 有负载
            m_inReq.ioBuf = new char[m_nContentLength];
            memset( m_inReq.ioBuf, 0 , m_nContentLength );
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readTCPContent()
{
    int rt = m_pReader->doRead( m_inReq.ioBuf + m_nReadOffset,
                            m_nContentLength - m_nReadOffset );

    if ( rt < 0 && ( errno == EWOULDBLOCK || errno == EINTR ) )
    {
        // 读阻塞或系统调用被中断
        return SUCCESSFUL;
    }
    else if ( rt < 0 )
    {
        // error
        ERROR_LOG( 
            "In BufferManager::readTCPContent, m_pReader->doRead error" );
        return FAILED;

    }
    else if ( 0 == rt )
    {
        // connection closed by opposition
        WARN_LOG(
            "In BufferManager::readTCPContent, connection closed by opposition" );
        return FAILED;
    }

    m_nReadOffset += ( uint32_t )rt;

    if ( m_nContentLength == m_nReadOffset )
    {
        // 读负载结束
        m_nReadOffset = 0;
        m_bNewPackage = true;

        // callback
        if ( NULL == m_pReadCallback )
        {
            WARN_LOG(
                "In BufferManager::readTCPContent, m_pReadCallback == NULL" );
            if( m_inReq.ioBuf != NULL )
            {
                delete [] m_inReq.ioBuf;
                m_inReq.ioBuf = NULL;
            }
            return SUCCESSFUL;
        }

        rt = m_pReadCallback->run( m_inReq );
        if( m_inReq.ioBuf != NULL )
        {
            delete [] m_inReq.ioBuf;
            m_inReq.ioBuf = NULL;
        }
        if ( rt < 0 )
        {
            ERROR_LOG(
                "In BufferManager::readContent, m_pReadCallback->run error" );
            return FAILED;
        }
    }

    return SUCCESSFUL;
}

int
BufferManager::readUDP()
{
    if ( NULL == m_pReader )
    {
        ERROR_LOG( "In BufferManager::readUDP, m_pReader == NULL" );
        return FAILED;
    }

    char* tempbuf = new char[UDPTEMPBUFLEN];
    m_nHeaderSize = HEADER_SIZE;
    m_inReq.ioBuf = NULL;

    memset( tempbuf, 0, UDPTEMPBUFLEN );
    memset( &m_inReq.m_msgHeader, 0 , m_nHeaderSize );

    int rt = m_pReader->doRead(
                 tempbuf, UDPTEMPBUFLEN, m_inReq.oppoAddr );

    if ( rt < 0 )
    {
        delete []tempbuf;
        tempbuf = NULL;
        return rt;
    }
    else if ( ( uint32_t )rt < m_nHeaderSize )
    {
        ERROR_LOG(
            "BufferManager::readUDP()"
            " Error, readNum < HEADER_SIZE" );
        delete []tempbuf;
        tempbuf = NULL;
        return FAILED;
    }

    memcpy( &m_inReq.m_msgHeader, tempbuf, m_nHeaderSize );
    m_inReq.ioBuf = new char[( uint32_t )rt - m_nHeaderSize + 1];
    memset( m_inReq.ioBuf, 0, ( uint32_t )rt - m_nHeaderSize + 1 );
    if ( m_inReq.m_msgHeader.length != rt - m_nHeaderSize )
    {
        ERROR_LOG(
            "BufferManager::readUDP()"
            " Error, date length error" );
    }
    memcpy( 
        m_inReq.ioBuf, 
        tempbuf + m_nHeaderSize, 
        ( uint32_t )rt - m_nHeaderSize );

    delete []tempbuf;
    tempbuf = NULL;
    
    if ( NULL == m_pReadCallback )
    {
        WARN_LOG(
            "In BufferManager::readUDP, m_pReadCallback == NULL" );
        if( m_inReq.ioBuf != NULL )
        {
            delete [] m_inReq.ioBuf;
            m_inReq.ioBuf = NULL;
        }
        return SUCCESSFUL;
    }

    rt = m_pReadCallback->run( m_inReq );
    m_bNewPackage = true;
    if( m_inReq.ioBuf != NULL )
    {
        delete [] m_inReq.ioBuf;
        m_inReq.ioBuf = NULL;
    }
    if ( rt < 0 )
    {
        ERROR_LOG(
            "In BufferManager::readUDP, m_pReadCallback->run error" );
        return FAILED;
    }

    return SUCCESSFUL;
}


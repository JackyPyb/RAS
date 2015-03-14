/*
 * =========================================================================
 *
 *       Filename:  UnxDmnAgent.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012-02-05 18:57:36
 *  Last Modified:  2012-02-05 18:57:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#include "common/comm/BufferManager.h"
#include "common/comm/UnxDmnReader.h"
#include "common/comm/UnxDmnWriter.h"
#include "common/comm/UnxDmnAgentReadCallback.h"
#include "common/comm/UnxDmnAgentWriteCallback.h"

UnxDmnAgent::UnxDmnAgent( AppProtoSpec* pSpec,
                          const UnxDmnSocketAddress& oppoAddr )
{
    m_iConnectTimes = -1;
    m_epollEvent.setHandlerID( getID() );
    m_oppoAddress = oppoAddr;
    m_Buffer.setAppProtoSpec( pSpec );
}

UnxDmnAgent::UnxDmnAgent( AppProtoSpec* pSpec,
                          const UnxDmnSocket& unxdmnsocket,
                          const UnxDmnSocketAddress& oppoAddr )
{
    m_iConnectTimes = -1;
    m_epollEvent.setHandlerID( getID() );

    m_socket = unxdmnsocket;
    m_oppoAddress = oppoAddr;
    m_epollEvent.setFd( m_socket.getFd() );

    m_Buffer.setAppProtoSpec( pSpec );
    m_Buffer.setReader( new UnxDmnReader( m_socket ) );
    m_Buffer.setWriter( new UnxDmnWriter( m_socket ) );
    m_Buffer.setReadCallback( new UnxDmnAgentReadCallback( this ) );
    m_Buffer.setWriteCallback( new UnxDmnAgentWriteCallback( this ) );

    if ( m_epollEvent.registerREvent() < 0 )
    {
        ERROR_LOG( "In UnxDmnAgent::UnxDmnAgent, "
                   "m_epollEvent.registerREvent() error" );
    }

    this->setState( CONNECTED );
}

UnxDmnAgent::~UnxDmnAgent()
{
    try
    {
        this->recycler();
    }
    catch ( ... )
    {
    }
}

void 
UnxDmnAgent::recycler( void )
{
    if ( m_bIsRecycler )
    {
        return;
    }
    m_bIsRecycler = true;

    if ( m_epollEvent.unregisterRWEvents() < 0 )
    {
        INFO_LOG( 
        	"UnxDmnAgent::recycler:m_epollEvent.unregisterRWEvents() error" );
    }
    m_epollEvent.setFd( -1 );
    if ( this->m_socket.close() < 0 )
    {
        INFO_LOG( "UnxDmnAgent::recycler:m_socket.close error" );
    }
}

int
UnxDmnAgent::init()
{
    return SUCCESSFUL;
}

int
UnxDmnAgent::connect()
{
    m_iConnectTimes += 1;

    int rt = m_socket.close();
    if ( rt < 0)
    {
    }
    rt = m_socket.socket();
    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnAgent::connect, m_socket.socket() error" );
        return FAILED;
    }
    rt = m_socket.setNonBlock();
    if ( rt < 0 )
    {
        ERROR_LOG(
            "In UnxDmnAgent::connect, m_socket.setNonBlock() error" );
        return FAILED;
    }

    m_epollEvent.setFd( m_socket.getFd() );
    m_Buffer.setReader( new UnxDmnReader( m_socket ) );
    m_Buffer.setWriter( new UnxDmnWriter( m_socket ) );
    m_Buffer.setReadCallback( new UnxDmnAgentReadCallback( this ) );
    m_Buffer.setWriteCallback( new UnxDmnAgentWriteCallback( this ) );

    if ( m_epollEvent.registerRWEvents() < 0 )
    {
        ERROR_LOG( "In UnxDmnAgent::connect, "
                   "m_epollEvent.registerRWEvents() error" );
        return FAILED;
    }

    if ( m_socket.connect( m_oppoAddress ) < 0 )
    {
        // NonBlock connect error
    }

    return SUCCESSFUL;
}

int
UnxDmnAgent::sendData()
{
    int rt = m_Buffer.write();

    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnAgent::sendData, m_Buffer.write() error" );
        return FAILED;
    }

    if ( 0 == m_Buffer.getSendBufferLength() )
    {
        // 发送缓冲区无数据，关闲写事件
        rt = m_epollEvent.closeWevent();

        if ( rt < 0 )
        {
            ERROR_LOG( "In UnxDmnAgent::sendData, "
                       "m_epollEvent.closeWevent() error" );
            return FAILED;
        }
    }

    return SUCCESSFUL;
}

int
UnxDmnAgent::recvData()
{
    int rt = m_Buffer.readUnxDmn();

    if ( rt < 0 )
    {
        ERROR_LOG( "In UnxDmnAgent::recvData, m_Buffer.read() error" );
        return FAILED;
    }

    return SUCCESSFUL;
}

void
UnxDmnAgent::setState( int state )
{
    m_iConnect = state;

    if ( state == CONNECTED )
    {
        if ( m_epollEvent.closeWevent() < 0 )
        {
            ERROR_LOG( "In UnxDmnAgent::setState, "
                       "m_epollEvent.closeWevent() error" );
        }
    }
}

void
UnxDmnAgent::writeToBuffer(
    char* pBuf, int len, DataContext* pContext )
{
    if ( 0 == m_Buffer.getSendBufferLength() )
    {
        if ( m_epollEvent.openWevent() < 0 )
        {
            ERROR_LOG( "In UnxDmnAgent::writeToBuffer, "
                       "m_epollEvent.openWevent() error" );
        }
    }

    m_Buffer.writeToBuffer( pBuf, len, pContext );
}


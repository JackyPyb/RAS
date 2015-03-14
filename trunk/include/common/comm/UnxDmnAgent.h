/*
 * =========================================================================
 *
 *       Filename:  UnxDmnAgent.h
 *
 *    Description:  使用Unix域套接字通信的Agent
 *
 *        Version:  1.0
 *        Created:  2012-01-13 01:11:16
 *  Last Modified:  2012-01-13 01:11:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#ifndef UNXDMNAGENT_H_
#define UNXDMNAGENT_H_

#include "common/comm/Agent.h"
#include "common/comm/EpollEvent.h"
#include "common/comm/UnxDmnSocketAddress.h"
#include "common/comm/UnxDmnSocket.h"
#include "common/comm/BufferManager.h"
#include "common/log/log.h"

struct BaseRequest;
struct InReq;

/**
 * @brief 使用Unix域套接字通信
 */
class UnxDmnAgent : public Agent
{
public:
    // pSpec必须是new出来的，并且在析构函数中不能释放，
    // 它被传递到m_Buffer中，将被m_Buffer释放
    UnxDmnAgent( AppProtoSpec* pSpec,
                 const UnxDmnSocketAddress& oppoAddr );

    UnxDmnAgent( AppProtoSpec* pSpec,
                 const UnxDmnSocket& unxdmnsocket,
                 const UnxDmnSocketAddress& oppoAddr );

    virtual ~UnxDmnAgent();

    int connect();

    virtual int init();

    virtual void recycler( void );

    virtual int sendData();

    virtual int recvData();

    virtual int recvReq( BaseRequest* )
    {
        return SUCCESSFUL;
    }

    virtual void readBack( InReq& )
    {

    }

    virtual int readCallback( const char* pHeader, uint32_t headerLen,
                              char* pContent, uint32_t contentLen )
    {
        return SUCCESSFUL;
    }

    virtual void writeBack( bool result )
    {
        if ( !result )
        {
            ERROR_LOG( "In UnxDmnAgent::writeBack, write error" );
        }
    }

    virtual int writeCallback( bool bResult,
                               const DataContext* pContext )
    {
        return SUCCESSFUL;
    }

    virtual int connectAfter( bool result )
    {
        return SUCCESSFUL;
    }

    void setState( int state );

    /**
     * @brief
     *
     * @param pBuf 必须是new出来的内存，并且不能释放，
     *              数据发送后由m_Buffer释放
     * @param len
     * @param pContext
     */
    virtual void writeToBuffer( char* pBuf, int len,
                                DataContext* pContext = NULL );

protected:
    UnxDmnSocket m_socket;
    UnxDmnSocketAddress m_oppoAddress;
    EpollEvent m_epollEvent;
    BufferManager m_Buffer;
};

#endif  // UNXDMNAGENT_H_


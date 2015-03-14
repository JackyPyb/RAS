#ifndef TCPAGENT_H_
#define TCPAGENT_H_

#include "common/comm/Agent.h"
#include "common/comm/BaseHeader.h"
#include "common/comm/EpollEvent.h"
#include "common/comm/BufferManager.h"
#include "common/comm/TCPSocket.h"

const SocketAddress InitAddr( "0", 0 );

struct InReq;

class TCPAgent: public Agent
{
public:
    TCPAgent();
    TCPAgent( const SocketAddress& );
    TCPAgent( const TCPSocket&, const SocketAddress& );
    virtual ~TCPAgent();

    int connect();
    virtual int init();
    virtual void recycler( void );
    virtual int sendData( void );
    virtual int recvData( void );
    virtual int doParse( void )
    {
        return SUCCESSFUL;
    }
    virtual int doPackage( void )
    {
        return SUCCESSFUL;
    }

    virtual void readBack( InReq& ) {}
    virtual void writeBack( bool ) {}
    virtual int connectAfter( bool );

    int writeToBuffer( char* buf , unsigned int len );

    virtual int getOppAddr( SocketAddress& sockAddr )
    {
        if ( InitAddr == m_addr )
        {
            return FAILED;
        }
        sockAddr = m_addr;
        return SUCCESSFUL;
    }

    const EpollEvent& getEpollEvent() const
    {
        return m_epollEvent;
    }
    void setState( int st );
    int getErrno();

    TCPSocket getSocket()const
    {
        return m_socket;
    }

protected:
    TCPSocket m_socket;
    SocketAddress m_addr;
    MsgHeader m_msgHeader;//maybe useless
    //add by fb 0407
    EpollEvent m_epollEvent;
    int m_iLen;
    BufferManager m_buffer;
    bool m_isActive;
};
#endif

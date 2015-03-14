#ifndef __UDPAGENT_H__
#define __UDPAGENT_H__

#include "common/comm/Agent.h"
#include "common/comm/EpollEvent.h"
#include "common/comm/BufferManager.h"
#include "common/comm/UDPSocket.h"
#include "common/comm/SocketAddress.h"

struct InReq;

class UDPAgent: public Agent
{
public:
    UDPAgent();
    UDPAgent( const SocketAddress& );
    virtual ~UDPAgent( void );

    virtual int init( const SocketAddress& );
    virtual int recvData( void );
    virtual int sendData( void );
    virtual void recycler( void );

    int readData( void );
    int writeData( void );
    int writeToBuffer( char* buf , unsigned int len );
    virtual void readBack( InReq& ) {}
    virtual void writeBack( bool result ) {}

protected:
    EpollEvent            m_epollEvent;
    UDPSocket           m_socket;
    SocketAddress     m_oppoAddr;
    BufferManager     m_buffer;
    int                        m_iLen;
    MsgHeader           m_msgHeader;
    bool                     m_bAgentType;
};

#endif

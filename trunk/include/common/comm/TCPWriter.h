#ifndef TCPWRITER_H_
#define TCPWRITER_H_

#include "Writer.h"
#include "TCPSocket.h"

class TCPWriter : public Writer
{
public:
    explicit TCPWriter( const TCPSocket& socket )
    {
        m_socket = socket;
    }

    virtual ~TCPWriter()
    {

    }

protected:
    virtual int doWriteInternal( 
        const struct iovec* pIovec, int iovcnt )
    {
        return m_socket.writev( pIovec, iovcnt );
    }

private:
    TCPSocket m_socket;
};

#endif //TCPWRITER_H_
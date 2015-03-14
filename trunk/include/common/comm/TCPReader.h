#ifndef TCPREADER_H_
#define TCPREADER_H_

#include "Reader.h"
#include "TCPSocket.h"

class TCPReader : public Reader
{
public:
    explicit TCPReader( const TCPSocket& socket )
    {
        m_socket = socket;
    }

    virtual ~TCPReader()
    {

    }

protected:
    virtual int doReadInternal( char* pBuf, size_t len )
    {
        return m_socket.read( pBuf, len );
    }

private:
    TCPSocket m_socket;
};

#endif  // TCPREADER_H_


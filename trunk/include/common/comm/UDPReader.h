#ifndef UDPREADER_H_
#define UDPREADER_H_

#include "Reader.h"
#include "UDPSocket.h"

class UDPReader : public Reader
{
public:
    explicit UDPReader( const UDPSocket& socket )
    {
        m_socket = socket;
    }

    virtual ~UDPReader()
    {

    }

protected:
    virtual int doReadInternal(
        char* pBuf, size_t len, struct sockaddr_in& addr )
    {
        return m_socket.read( pBuf, len, addr );
    }

private:
    UDPSocket m_socket;
};

#endif  // UDPREADER_H_


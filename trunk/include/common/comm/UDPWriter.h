#ifndef UDPWRITER_H_
#define UDPWRITER_H_

#include "common/comm/Writer.h"
#include "common/comm/UDPSocket.h"
#include "common/comm/Error.h"

class UDPWriter : public Writer
{
public:
    explicit UDPWriter( const UDPSocket& socket )
    {
        m_socket = socket;
    }

    virtual ~UDPWriter()
    {

    }

    int doWrite( list<Iov>* pSendIovList );

protected:

    int doWriteInside(
        const char* buf, size_t len, const struct sockaddr_in& cliaddr )
    {
        return m_socket.write( buf, len, cliaddr );
    }
private:
    UDPSocket m_socket;
};

#endif //UDPWRITER_H_


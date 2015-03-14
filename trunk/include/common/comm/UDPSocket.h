#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <sys/uio.h>
#include <netinet/in.h>

class SocketAddress;

class UDPSocket
{
public:
    static int ipType;

public:
    UDPSocket();
    ~UDPSocket();

    int generateSocket( void );
    int bind( const SocketAddress& servaddr );
    int read( char* buf, size_t len, struct sockaddr_in& cliaddr );
    int write( const char* buf, size_t len, const struct sockaddr_in& cliaddr );
    int getFd( void ) const;
    int close( void );

    int setNonblock( void );
    int enableReuseaddr( void );
    int enableBroadcast( void );

private:
    int m_sockFd;
};

#endif

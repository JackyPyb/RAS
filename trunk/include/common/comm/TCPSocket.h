#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <sys/uio.h>

class SocketAddress;

class TCPSocket
{
public:
    static int ipType;

public:
    TCPSocket();
    TCPSocket( int fd );
    ~TCPSocket();

    int generateSocket( void );
    int bind( const SocketAddress& );
    int listen( int qs );
    int connect( const SocketAddress& );
    int accept( SocketAddress& );
    int read( char* buf, size_t len );
    int readn( char* vptr, unsigned int n );
    int write( const char* buf, size_t len );
    int writev( const struct iovec*, size_t );
    int getFd( void ) const;
    int close( void );

    int setNonblock( void );
    int enableReuseaddr( void );
    int disableLinger( void );
    int disableNagle( void );
    int setKeepAlive( void );

    //for debug
    int Peek( char* buf, size_t size );
    //for debug end
    void ThrowData( unsigned int size );

private:
    int m_sockFd;
};

#endif

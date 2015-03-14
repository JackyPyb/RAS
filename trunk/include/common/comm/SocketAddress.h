#ifndef SOCKETADDRESS_H_
#define SOCKETADDRESS_H_

#include <string>
#include <netinet/in.h>

using namespace std;

class SocketAddress
{
public:
    static int ipType;

public:
    SocketAddress();
    SocketAddress( const char* buf, unsigned short p );
    ~SocketAddress();

    void setAddress( const char*, unsigned short );
    const char* getIP( void )const
    {
        return m_ip.c_str();
    };
    unsigned short getPort( void )const
    {
        return m_port;
    };
    sockaddr_in getAddr();
    bool getAddr( sockaddr_in& addr );  // added by niexw
    void setPort( unsigned short );
    string& convertToString( void );
    bool ifAnyAddr( void )const;
    bool operator == ( const SocketAddress& )const;
    SocketAddress& operator = ( const SocketAddress& );

private:
    string          m_ip;
    unsigned short  m_port;
    string          m_str;
};

#endif

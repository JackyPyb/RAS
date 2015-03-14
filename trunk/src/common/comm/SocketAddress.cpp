#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common/comm/SocketAddress.h"
#include "common/comm/BaseHeader.h"

const size_t IPV4LEN = 15;
const size_t IPV6LEN = 39;
const size_t MAXSOCKADDRLEN = 100;
const char* ANYADDR = "0";
const char* IPV4_ANYADDR = "0.0.0";
const char* IPV6_ANYADDR = "::1";

int SocketAddress::ipType = IPV4;

SocketAddress::SocketAddress():
    m_port( 0 ), m_str( "" )
{
    if ( ipType == IPV4 )
    {
        m_ip = IPV4_ANYADDR;
    }
    else
    {
        m_ip = IPV6_ANYADDR;
    }
}

SocketAddress::SocketAddress( const char* buf, unsigned short p )
{
    m_port = p;
    if ( ipType == IPV4 )
        if ( strlen( buf ) > IPV4LEN || strcmp( buf, ANYADDR ) == 0 )
        {
            m_ip = IPV4_ANYADDR;
        }
        else
        {
            m_ip = string( buf );
        }
    else
    {
        if ( strlen( buf ) > IPV6LEN || strcmp( buf, ANYADDR ) == 0 )
        {
            m_ip = IPV6_ANYADDR;
        }
        else
        {
            m_ip = string( buf );
        }
    }
}

SocketAddress::~SocketAddress()
{
}

void
SocketAddress::setPort( unsigned short p )
{
    m_port = p;
}

void
SocketAddress::setAddress( const char* buf, unsigned short p )
{
    m_port = p;
    if ( ipType == IPV4 )
    {
        if ( strlen( buf ) <= IPV4LEN && strcmp( buf, ANYADDR ) != 0 )
        {
            m_ip = string( buf );
        }
    }
    else
    {
        if ( strlen( buf ) <=  IPV6LEN && strcmp( buf, ANYADDR ) != 0 )
        {
            m_ip = string( buf );
        }
    }
}

string&
SocketAddress::convertToString( void )
{
    char buf[ MAXSOCKADDRLEN ] = { 0 };
    sprintf( buf, "%s %u", m_ip.c_str(), m_port );
    m_str = buf;
    return m_str;
}

bool
SocketAddress::ifAnyAddr( void )const
{
    if ( ipType == IPV4 )
        if ( IPV4_ANYADDR == m_ip )
        {
            return true;
        }
        else
        {
            return false;
        }
    else if ( IPV6_ANYADDR == m_ip )
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool
SocketAddress::operator == ( const SocketAddress& h )const
{
    if ( m_ip == h.m_ip && m_port == h.m_port )
    {
        return true;
    }
    else
    {
        return false;
    }
}

SocketAddress&
SocketAddress::operator=( const SocketAddress& h )
{
    m_ip = h.m_ip;
    m_port = h.m_port;
    m_str = "";
    return *this;
}

sockaddr_in SocketAddress::getAddr()
{
    struct sockaddr_in addr;
    memset( &addr, 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr( m_ip.c_str() );
    addr.sin_port = htons( m_port );
    return addr;
}

bool SocketAddress::getAddr( sockaddr_in& addr )
{
    memset( &addr, 0, sizeof( addr ) );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( m_port );
    return inet_pton( 
        AF_INET, m_ip.c_str(), &addr.sin_addr ) == 1 ? true : false;
}

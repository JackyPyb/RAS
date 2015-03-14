/*
 * =========================================================================
 *
 *       Filename:  UnxDmnSocketAddress.h
 *
 *    Description:  UnixÓòÌ×½Ó×ÖµØÖ··â×°
 *
 *        Version:  1.0
 *        Created:  2012-01-12 19:42:29
 *  Last Modified:  2012-01-12 19:42:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =========================================================================
 */

#ifndef UNXDMNSOCKETADDRESS_H_
#define UNXDMNSOCKETADDRESS_H_

#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

/**
 * @brief UnixÓòÌ×½Ó×ÖµØÖ·
 */
class UnxDmnSocketAddress
{
public:
    UnxDmnSocketAddress() :
        m_cstrPath( m_strPath ),
        m_cSockAddr( m_sockAddr )
    {
        memset( &m_sockAddr, 0, sizeof( m_sockAddr ) );
        m_sockAddr.sun_family = AF_LOCAL;
    }

    explicit UnxDmnSocketAddress( const std::string& path ) :
        m_cstrPath( m_strPath ),
        m_cSockAddr( m_sockAddr )
    {
        m_strPath = path;
        memset( &m_sockAddr, 0, sizeof( m_sockAddr ) );
        m_sockAddr.sun_family = AF_LOCAL;
        memcpy( m_sockAddr.sun_path, m_strPath.c_str(), m_strPath.length() );
    }

    UnxDmnSocketAddress( const UnxDmnSocketAddress& addr ) :
        m_cstrPath( m_strPath ),
        m_cSockAddr( m_sockAddr )
    {
        m_strPath = addr.m_strPath;
        m_sockAddr = addr.m_sockAddr;
    }

    virtual ~UnxDmnSocketAddress()
    {

    }

    UnxDmnSocketAddress& operator=( const UnxDmnSocketAddress& addr )
    {
        m_strPath = addr.m_strPath;
        m_sockAddr = addr.m_sockAddr;

        return *this;
    }

    void setAddress( const std::string& path )
    {
        m_strPath = path;
        memcpy( m_sockAddr.sun_path, m_strPath.c_str(), m_strPath.length() );
    }

    void setAddress( const struct sockaddr_un& addr )
    {
        m_sockAddr = addr;
        m_strPath = m_sockAddr.sun_path;
    }

public:
    const std::string& m_cstrPath;
    const struct sockaddr_un& m_cSockAddr;

private:
    std::string m_strPath;
    struct sockaddr_un m_sockAddr;
};

#endif  // UNXDMNSOCKETADDRESS_H_


/*
 * =============================================================================
 *
 *       Filename:  ConnectionFactory.cpp
 *
 *    Description:  DB Connection Factory
 *
 *        Version:  1.0
 *        Created:  2011-05-07 12:23:19
 *  Last Modified:  2011-05-07 12:23:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */


#include <cstddef>
#include <cstdlib>
#include <sqlext.h>

#include "common/odbc/ConnectionFactory.h"
#include "common/odbc/Connection.h"
#include "common/odbc/ConnectionImpl.h"
#include "common/odbc/odbcerr.h"
#include "common/odbc/odbctypes.h"
#include "common/log/log.h"
#include "common/comm/Error.h"

// handle for ODBC environment
SQLHENV ConnectionFactory::m_env = SQL_NULL_HENV;

// constructor
ConnectionFactory::ConnectionFactory() :
    m_bInited( false ),
    m_pooling( ODBC_CP_ONE_PER_HENV ),
    m_version( ODBC_V3 )
{

}

// destructor
ConnectionFactory::~ConnectionFactory()
{
    try
    {
        SQLRETURN ret = SQLFreeHandle( SQL_HANDLE_ENV, m_env );
        if ( ret != SQL_SUCCESS)
        {
            ERROR_LOG(
                "ConnectionFactory::~ConnectionFactory SQLFreeHandle error");
        }
    }
    catch ( ... )
    {
    }
}

int ConnectionFactory::doSetEnvAttr( SQLINTEGER attr,
                                     SQLPOINTER valuePtr,
                                     SQLINTEGER strLen ) const
{
    SQLRETURN ret;

    ret = SQLSetEnvAttr( m_env,
                         attr,
                         valuePtr,
                         strLen );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( 
            odbc::getODBCError( 
                "SQLSetEnvAttr", SQL_HANDLE_ENV, m_env ).c_str() );
        return FAILED;
    }
    return SUCCESSFUL;
}

int ConnectionFactory::init()
{
    if ( m_bInited )
    {
        return SUCCESSFUL;
    }

    m_bInited = true;

    // allocate ODBC environment handle
    SQLRETURN ret = SQLAllocHandle(
                        SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_env );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( odbc::getODBCError(
                       "SQLAllocHandle", SQL_HANDLE_ENV, m_env ).c_str() );
        return FAILED;
    }

    //set ODBC version
    int rt = doSetEnvAttr( SQL_ATTR_ODBC_VERSION,
                           reinterpret_cast<SQLPOINTER>( m_version ),
                           0 );
    if ( -1 == rt )
    {
        return FAILED;
    }

    //connection pooling
    rt = doSetEnvAttr( SQL_ATTR_CONNECTION_POOLING,
                       reinterpret_cast<SQLPOINTER>( m_pooling ),
                       0 );
    if ( -1 == rt )
    {
        return FAILED;
    }

    return SUCCESSFUL;
}

int ConnectionFactory::setConnectionPooling(
    uint32_t pooling )
{

    m_pooling = pooling;

    if ( !m_bInited )
    {
        return SUCCESSFUL;
    }

    return doSetEnvAttr( SQL_ATTR_CONNECTION_POOLING,
                         reinterpret_cast<SQLPOINTER>( m_pooling ),
                         0 );
}

int ConnectionFactory::setVersion( uint32_t version )
{
    m_version = version;

    if ( !m_bInited )
    {
        return SUCCESSFUL;
    }

    return doSetEnvAttr( SQL_ATTR_ODBC_VERSION,
                         reinterpret_cast<SQLPOINTER>( m_version ),
                         0 );
}

// Factory method
Connection* ConnectionFactory::createConnection() const
{
    SQLRETURN ret;
    SQLHDBC dbc;
    Connection* pConn = NULL;

    // if not initialized
    if ( !m_bInited )
    {
        DEBUG_LOG( "ConnectionFactory has not been initialized." );
        return pConn;
    }

    // allocate handle
    ret = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &dbc );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        DEBUG_LOG( odbc::getODBCError(
                       "SQLAllocHandle", SQL_HANDLE_ENV, m_env ).c_str() );
        return pConn;
    }

    // new Connection
    pConn = new ConnectionImpl( dbc );

    return pConn;
}


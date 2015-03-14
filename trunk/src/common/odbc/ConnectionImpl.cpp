/*
 * =============================================================================
 *
 *       Filename:  ConnectionImpl.cpp
 *
 *    Description:  数据库连接实现
 *
 *        Version:  1.0
 *        Created:  2011-05-08 21:00:03
 *  Last Modified:  2011-05-08 21:00:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include "common/odbc/ConnectionImpl.h"
#include "common/odbc/Statement.h"
#include "common/odbc/odbcerr.h"
#include "common/comm/Error.h"
#include "common/log/log.h"

// constructor
ConnectionImpl::ConnectionImpl( SQLHDBC& conn ) :
    m_hDbc( conn )
{

}


// destructor
ConnectionImpl::~ConnectionImpl()
{
    try
    {
        int rt = SQLDisconnect( m_hDbc );
        if ( rt < 0 )
        {
            ERROR_LOG(
                "ConnectionImpl::~ConnectionImpl SQLDisconnect error");
            return;
        }
        SQLRETURN ret = SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
        if ( ret != SQL_SUCCESS)
        {
            ERROR_LOG(
                "ConnectionImpl::~ConnectionImpl SQLFreeHandle error");
        }
    }
    catch ( ... )
    {
    }
}

// close connection, after call this member, all operations on this
// object are invaid
void ConnectionImpl::close()
{
    delete this;
}


// factory method, create Statement
Statement* ConnectionImpl::createStatement()
{
    SQLHSTMT stmt;
    SQLRETURN ret;
    Statement* pStmt = NULL;

    // allocate statement handle
    ret = SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &stmt );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( odbc::getODBCError(
                       "SQLAllocHandle", SQL_HANDLE_DBC, m_hDbc ).c_str() );
        return pStmt;
    }

    // new Statement
    pStmt = new Statement( stmt );

    return pStmt;
}

int ConnectionImpl::doSetConnectAttr( SQLINTEGER attr,
                                      SQLPOINTER valuePtr,
                                      SQLINTEGER strLen )
{
    SQLRETURN ret;

    ret = SQLSetConnectAttr( m_hDbc,
                             attr,
                             valuePtr,
                             strLen );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( odbc::getODBCError(
                       "SQLSetConnectAttr", SQL_HANDLE_DBC, m_hDbc ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

int ConnectionImpl::setAutoCommit( bool ac )
{
    SQLUINTEGER sqlAc = true;

    if ( ac )
    {
        sqlAc = SQL_AUTOCOMMIT_ON;
    }
    else
    {
        sqlAc = SQL_AUTOCOMMIT_OFF;
    }

    int rt = doSetConnectAttr( SQL_ATTR_AUTOCOMMIT,
                               reinterpret_cast<SQLPOINTER>( sqlAc ),
                               0 );
    if ( -1 == rt )
    {
        return FAILED;
    }

    return SUCCESSFUL;
}

int ConnectionImpl::connect( const std::string& dsn )
{
    m_DSN = dsn;

    // connect to database
    SQLRETURN ret = SQLDriverConnect(
              m_hDbc,          // ConnectionHandle  SQLHDBC
              NULL,            // WindowHandle  SQLHWND
              // InConnectionString  SQLCHAR*
              reinterpret_cast<SQLCHAR*>( const_cast<char*>( dsn.c_str() ) ),
              SQL_NTS,         // StringLength1 SQLCHAR*
              NULL,                   // OutConnectionString  SQLCHAR*
              0,                      // BufferLength  SQLSMALLINT
              NULL,                   // StringLength2Ptr  SQLSMALLINT*
              SQL_DRIVER_NOPROMPT     // DriverCompletion  SQLUSMALLINT
          );

    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( odbc::getODBCError(
                       "SQLDriverConnect", SQL_HANDLE_DBC, m_hDbc ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}


/*
 * =============================================================================
 *
 *       Filename:  Statement.cpp
 *
 *    Description:  数据库Statement句柄实现
 *
 *        Version:  1.0
 *        Created:  2011-05-09 12:18:43
 *  Last Modified:  2011-05-25 11:27:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include "common/odbc/Statement.h"
#include "common/odbc/odbcerr.h"
#include "common/comm/Error.h"
#include "common/log/log.h"

using std::string;

// hidden constructor
Statement::Statement( SQLHSTMT& stmt ):
    m_stmt( stmt )
{

}

// destructor
Statement::~Statement()
{
    SQLRETURN ret;
    ret = SQLCloseCursor( m_stmt );
    if ( ret != SQL_SUCCESS)
    {
    }
    ret = SQLFreeHandle( SQL_HANDLE_STMT, m_stmt );
    if ( ret != SQL_SUCCESS)
    {
    }
}


// close Statement, free resources
void
Statement::close()
{
    delete this;
}

// close cursor opened on this statement
int
Statement::closeCursor()
{
    SQLRETURN ret = SQLCloseCursor( m_stmt );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while closing cursor." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLCloseCursor", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

// prepare an sql string for executing
int
Statement::prepare( const string& sql )
{
    SQLRETURN ret = SQLPrepare(
              m_stmt,
              reinterpret_cast<SQLCHAR*>( const_cast<char*>( sql.c_str() ) ),
              (int)sql.length() );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while preparing statment." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLPrepare", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

int
Statement::bindColumnByColumn()
throw ( ODBCException )
{
    SQLRETURN ret = SQLSetStmtAttr(
              m_stmt,
              SQL_ATTR_ROW_BIND_TYPE,
              reinterpret_cast<SQLPOINTER>( SQL_BIND_BY_COLUMN ),
              0  // string length
          );

    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG(
            "ODBC: Error occurred while setting column-wise binding(bind a column)." );
        ERROR_LOG(
            odbc::getODBCError(
                "SQLSetStmtAttr", SQL_HANDLE_STMT, m_stmt ).c_str() );
        // throw exception
        throw ODBCException(
            "Error occurred while setting column-wise binding"
            "(bind a column)",
            __FILE__,
            __LINE__ );
    }

    return SUCCESSFUL;
}

int
Statement::bindColumnByRow( long structLen )
throw ( ODBCException )
{
    SQLRETURN ret = SQLSetStmtAttr(
              m_stmt,
              SQL_ATTR_ROW_BIND_TYPE,
              reinterpret_cast<SQLPOINTER>( structLen ),
              0 );

    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG(
            "ODBC: Error occurred while setting row-wise binding(bind a column)." );
        ERROR_LOG(
            odbc::getODBCError(
                "SQLSetStmtAttr", SQL_HANDLE_STMT, m_stmt ).c_str() );
        // throw exception
        throw ODBCException(
            "Error occurred while setting row-wise binding"
            "(bind a column)",
            __FILE__,
            __LINE__ );
    }

    return SUCCESSFUL;
}


int
Statement::bindColumn(
    int colIdx,
    ODBCType targetType,
    void* targetValPtr,
    long bufLen,
    long* strLenOrIndPtr ) throw ( ODBCBindColumnException )
{
    SQLRETURN ret = SQLBindCol(
              m_stmt,
              colIdx,
              ODBCCTypes[targetType],
              targetValPtr,
              bufLen,
              strLenOrIndPtr );

    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while binding column." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLBindCol", SQL_HANDLE_STMT, m_stmt ).c_str() );
        // throw exception
        throw ODBCBindColumnException(
            "Error occurred while binding column",
            __FILE__,
            __LINE__,
            colIdx );
    }

    return SUCCESSFUL;
}

int
Statement::bindParamByColumn()
throw ( ODBCException )
{
    SQLRETURN ret = SQLSetStmtAttr(
              m_stmt,
              SQL_ATTR_PARAM_BIND_TYPE,
              reinterpret_cast<SQLPOINTER>( SQL_PARAM_BIND_BY_COLUMN ),
              0 );

    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG(
            "ODBC: Error occurred while setting column-wise binding"
            "(bind a parameter)" );
        ERROR_LOG(
            odbc::getODBCError(
                "SQLSetStmtAttr", SQL_HANDLE_STMT, m_stmt ).c_str() );
        // throw exception
        throw ODBCException(
            "Error occurred while setting column-wise binding(bind a parameter",
            __FILE__,
            __LINE__ );
    }

    return SUCCESSFUL;
}

int
Statement::bindParamByRow( int structLen )
throw ( ODBCException )
{
    SQLRETURN ret = SQLSetStmtAttr(
              m_stmt,
              SQL_ATTR_PARAM_BIND_TYPE,
              reinterpret_cast<SQLPOINTER>( structLen ),
              0 );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG(
            "ODBC: Error occurred while setting row-wise binding"
            "(bind a parameter)" );
        ERROR_LOG( odbc::getODBCError(
                       "SQLSetStmtAttr", SQL_HANDLE_STMT, m_stmt ).c_str() );
        // throw exception
        throw ODBCException(
            "Error occurred while setting row-wise binding(bind a parameter",
            __FILE__,
            __LINE__ );
    }

    return SUCCESSFUL;
}

int
Statement::bindParameter(
    int paramIdx,
    ODBCParamInputOutputType ioType,
    ODBCType paramType,
    void* paramValPtr,
    long bufLen,
    long* strLenOrIndPtr ) throw ( ODBCBindParameterException )
{
    SQLULEN columnSize = 0;
    if ( ODBC_STRING == paramType )
    {
        columnSize = (SQLULEN)bufLen - 1;
    }
    else if ( ODBC_FLOAT == paramType || ODBC_DOUBLE == paramType )
    {
        columnSize = 15;
    }

    SQLRETURN ret = SQLBindParameter(
              m_stmt,
              paramIdx,
              ODBCPARAMIOTYPES[ioType],
              ODBCCTypes[paramType],
              ODBCSQLTypes[paramType],
              columnSize,
              0,  // Decimal Digits
              paramValPtr,
              bufLen,
              strLenOrIndPtr );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while binding parameter." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLBindParameter", SQL_HANDLE_STMT, m_stmt ).c_str() );
        // throw exception
        throw ODBCBindParameterException(
            "Error occurred while binding paramter",
            __FILE__,
            __LINE__,
            paramIdx );
    }

    return SUCCESSFUL;
}

// execute an sql directly
int Statement::execute( const string& sql )
{
    SQLRETURN ret = SQLExecDirect(
              m_stmt,
              reinterpret_cast<SQLCHAR*>( const_cast<char*>( sql.c_str() ) ),
              (int)sql.length() );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while executing a sql directly." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLExecDirect", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

// execute an prepared sql, should call prepare() first
int Statement::execute()
{
    SQLRETURN ret;

    ret = SQLExecute( m_stmt );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while executing a prepared statement." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLExecute", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

// get data of specified column after fetch
int Statement::getData(
    SQLUSMALLINT colNum,
    SQLSMALLINT targetType,
    SQLPOINTER targetValPtr,
    SQLLEN bufLen,
    SQLLEN* strLen_or_indPtr )
{
    SQLRETURN ret;

    ret = SQLGetData(
              m_stmt,
              colNum,
              targetType,
              targetValPtr,
              bufLen,
              strLen_or_indPtr );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG( "ODBC: Error occurred while getting column data." );
        ERROR_LOG( odbc::getODBCError(
                       "SQLGetData", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}


// get columns of result set
int Statement::getNumResultCols( int* valPtr )
{
    SQLRETURN ret = SQLNumResultCols( 
        m_stmt, 
        reinterpret_cast<SQLSMALLINT*>( valPtr ) );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG(
            "ODBC: Error occurred while getting number of columns of result set." );
        ERROR_LOG(
            odbc::getODBCError(
                "SQLNumResultCols", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

// get rows of result set
int Statement::getNumResultRows( long* valPtr )
{
    SQLRETURN ret;

    ret = SQLRowCount( m_stmt, reinterpret_cast<SQLLEN*>( valPtr ) );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        ERROR_LOG(
            "ODBC: Error occurred while getting number of rows of result set." );
        ERROR_LOG(
            odbc::getODBCError(
                "SQLRowCount", SQL_HANDLE_STMT, m_stmt ).c_str() );
        return FAILED;
    }

    return SUCCESSFUL;
}

// fetch result set
int Statement::fetch()
{
    SQLRETURN ret;

    ret = SQLFetch( m_stmt );
    if ( !SQL_SUCCEEDED( ret ) )
    {
        if ( SQL_NO_DATA != ret )
        {
            ERROR_LOG( "ODBC: Error occurred while fetching result set." );
            ERROR_LOG(
                odbc::getODBCError(
                    "SQLFetch", SQL_HANDLE_STMT, m_stmt ).c_str() );
            return FAILED;
        }
        return ODBC_NO_DATA;
    }

    return SUCCESSFUL;
}


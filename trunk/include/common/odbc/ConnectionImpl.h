/*
 * =============================================================================
 *
 *       Filename:  ConnectionImpl.h
 *
 *    Description:  数据库连接实现
 *
 *        Version:  1.0
 *        Created:  2011-05-07 16:26:43
 *  Last Modified:  2011-05-07 16:26:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#ifndef COMMON_ODBC_CONNECTIONIMPL_H_
#define COMMON_ODBC_CONNECTIONIMPL_H_

#include "common/odbc/Connection.h"
#include "common/odbc/ConnectionFactory.h"

class Statement;

/**
 * @brief ConnectionImpl to Database via ODBC
 */
class ConnectionImpl : public Connection
{
    friend class ConnectionFactory;
public:
    virtual Statement* createStatement();  // factory method
    virtual void close();  // close the connection, free resources
    virtual int setAutoCommit( bool ac );
    virtual int connect( const std::string& dsn );

protected:
    // can't new a ConnectionImpl object
    ConnectionImpl();
    ConnectionImpl( SQLHDBC& conn );
    // can't delete
    virtual ~ConnectionImpl();

    int doSetConnectAttr( SQLINTEGER attr,
                          SQLPOINTER valuePtr,
                          SQLINTEGER strLen );

private:
    SQLHDBC m_hDbc;
    std::string m_DSN;

};

#endif  // COMMON_ODBC_CONNECTIONIMPL_H_


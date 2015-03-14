/*
 * =============================================================================
 *
 *       Filename:  Connection.h
 *
 *    Description:  数据库连接抽象类
 *
 *        Version:  1.0
 *        Created:  2011-05-07 15:36:55
 *  Last Modified:  2011-05-07 15:36:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */


#ifndef COMMON_ODBC_CONNECTION_H_
#define COMMON_ODBC_CONNECTION_H_

#include <sql.h>
#include <string>

class Statement;

/**
 * @brief Connection to Database via ODBC
 */
class Connection
{
public:
    /**
     * @brief 工厂方法，创建Statement对象
     *
     * @return 指向创建的Statement对象的指针
     */
    virtual Statement* createStatement() = 0;  // factory method

    /**
     * @brief 关闭连接
     */
    virtual void close() = 0;  // close the connection, free resources

    /**
     * @brief 设置自动提交(set before or after connection? either)
     *
     * @param ac
     *      - true 自动提交
     *      - false 不自动提交
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    virtual int setAutoCommit( bool ac ) = 0;

    /**
     * @brief 连接数据库
     *
     * @param dsn Data Source Name
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    virtual int connect( const std::string& dsn ) = 0;

protected:
    Connection() {}  // can't new
    // can't copy
    Connection( const Connection& conn );
    Connection& operator=( const Connection& conn );

    virtual ~Connection() {}  // can't delete
};

#endif  // COMMON_ODBC_CONNECTION_H_


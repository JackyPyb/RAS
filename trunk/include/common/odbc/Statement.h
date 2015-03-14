/*
 * =============================================================================
 *
 *       Filename:  Statement.h
 *
 *    Description:  数据库Statement句柄实现，类似数据库中的游标
 *
 *        Version:  1.0
 *        Created:  2011-05-08 21:21:35
 *  Last Modified:  2011-05-08 21:21:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#ifndef COMMON_ODBC_STATEMENT_H_
#define COMMON_ODBC_STATEMENT_H_

#include <cstddef>
#include <sql.h>
#include <sqlext.h>

#include "common/odbc/odbctypes.h"
#include "common/odbc/ODBCException.h"
#include "common/odbc/ODBCBindParameterException.h"
#include "common/odbc/ODBCBindColumnException.h"

class Connection;

class Statement
{
    friend class ConnectionImpl;

public:
    /**
     * @brief 关闭游标，释放句柄
     */
    void close();

    /**
     * @brief 关闭与句柄关联的游标，以便重用句柄
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int closeCursor();

    /**
     *
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    /**
     * @brief 绑定列，以便从数据库fetch数据到绑定的缓冲区中
     *
     * @param colIdx 列序号，从1开始
     * @param targetType 数据类型
     * @param targetValPtr 缓冲区地址
     * @param bufLen 缓冲区长度
     * @param strLenOrIndPtr 实际返回的数据长度，
     *      或者对返回数据的指（比如为NULL）
     *
     * @return
     *      -1 - fail
     *       0 - success
     *
     * @throw ODBCBindColumnException
     *      列绑定异常
     */
    int bindColumn(
        int colIdx,
        ODBCType targetType,
        void* targetValPtr,
        long bufLen,
        long* strLenOrIndPtr ) throw ( ODBCBindColumnException );

    /**
     * @brief 按列方式绑定结果参数（默认）
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int bindColumnByColumn() throw ( ODBCException );

    /**
     * @brief 按行方式绑定结果参数
     *
     * @param structLen 结果参数结构体size
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int bindColumnByRow( long structLen ) throw ( ODBCException );

    /**
     * @brief 为一个prepare语句绑定参数
     *
     * @param paramIdx 参数位置
     * @param ioType 参数输入输出类型
     * @param paramType 参数类型
     * @param paramValPtr 参数
     * @param bufLen 参数缓冲区长度
     * @param strLenOrIndPtr 参数值指示器或者参数长度
     *
     * @return
     *      -1 - fail
     *       0 - success
     *
     * @throw ODBCBindParameterException
     *      参数绑定异常
     */
    int bindParameter(
        int paramIdx,
        ODBCParamInputOutputType ioType,
        ODBCType paramType,
        void* paramValPtr,
        long bufLen,
        long* strLenOrIndPtr ) throw ( ODBCBindParameterException );

    /**
     * @brief 参数绑定方式设置为按列绑定（默认）
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int bindParamByColumn() throw ( ODBCException );

    /**
     * @brief 参数绑定方式设置为按行绑定
     *
     * @param structLen 用于绑定的结构体size
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int bindParamByRow( int structLen ) throw ( ODBCException );

    /**
     * @brief Prepare一条SQL语句以便之后execute，
     *      通常使用占位符以便可以动态绑定参数，
     *       如果某条语句要重复执行多次，则这种方式可以提高效率
     *
     * @param sql sql语句
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int prepare( const std::string& sql );

    /**
     * @brief 直接执行一条sql语句，通常用于执行简单语句
     *
     * @param sql 要执行的SQL语句
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int execute( const std::string& sql );

    /**
     * @brief 执行之前prepare的语句，
     *      调用此方法之前必须先prepare
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int execute();

    /**
     * @brief 获取结果集的列数
     *
     * @param valPtr 结果缓冲区
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int getNumResultCols( int* valPtr );

    /**
     * @brief 获得结果集行数
     *
     * @param valPtr 结果缓冲区，必须是long类型
     *
     * @return
     *      -1 - fail
     *       0 - success
     */
    int getNumResultRows( long* valPtr );

    /**
     * @brief 从数据库取回一条记录
     *
     * @return
     *      -1 - failed
     *       0 - success and still have data
     *      ODBC_NO_DATA - success and have no data any more
     */
    int fetch();

    int getData(
        SQLUSMALLINT colNum,
        SQLSMALLINT targetType,
        SQLPOINTER targetValPtr,
        SQLLEN bufLen,
        SQLLEN* retLen ); // get data after fetch

protected:
    // can't new
    Statement( SQLHSTMT& stmt );
    // can't copy
    Statement( const Statement& stmt );
    Statement& operator=( const Statement& stmt );
    // can't delete
    ~Statement();

private:
    SQLHSTMT m_stmt;
};

#endif  // COMMON_ODBC_STATEMENT_H_


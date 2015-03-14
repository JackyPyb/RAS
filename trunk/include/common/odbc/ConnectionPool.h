/*
 * =====================================================================================
 *
 *       Filename:  ConnectionPool.h
 *
 *    Description:  数据库连接池定义
 *
 *        Version:  1.0
 *        Created:  04/25/2011 07:46:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#ifndef COMMON_ODBC_CONNECTIONPOOL_H_
#define COMMON_ODBC_CONNECTIONPOOL_H_

#include <list>

#include "common/sys/noncopyable.h"
#include "common/sys/Mutex.h"
#include "common/sys/Cond.h"
#include "common/odbc/Connection.h"

class Statement;

/**
 * @brief 最大空闲连接数
 */
#define CONN_MAX_IDLE_DEFAULT 30
/**
 * @brief 最大连接数
 */
#define CONN_MAX_TOTAL_DEFAULT 100
/**
 * @brief 最大空闲时间
 */
#define CONN_MAX_IDLE_TIMES 10

/**
 * @brief 数据库连接池定义
 */
class ConnectionPool : public noncopyable
{
public:
    /**
     * @brief 构造函数
     *
     * @param dsn 数据源名称（连接字串）
     * @param maxIdle 最大空闲连接数
     * @param maxTotal 最大连接数
     */
    ConnectionPool(
        const std::string& dsn,
        int maxIdle = CONN_MAX_IDLE_DEFAULT,
        int maxTotal = CONN_MAX_TOTAL_DEFAULT );

    ~ConnectionPool();

    /**
     * @brief 连接池开始工作（建立到数据库的连接）
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    int start();

    /**
     * @brief 从连接池获取连接
     *
     * @return 连接
     */
    Connection* getConnection();

private:
    /**
     * @brief 将连接释放回连接池（由ConnectionImplCP调用）
     *
     * @param pConn 由ConnectionFactory创建的连接
     */
    void releaseToConnectionPool( Connection* pConn );

    /**
     * @brief 从list头部获得一个空闲连接
     *
     * @return
     *      - NULL 没有空闲连接
     */
    Connection* getIdleConnection();

    /**
     * @brief 没有空闲连接，创建一个新连接
     *
     * @return
     *      - NULL 已达到最大连接数，或者创建新连接失败
     */
    Connection* newConnection();

    /**
     * @brief 没有空闲连接并且达到最大连接数，等待直到有空闲连接
     *
     * @return
     */
    Connection* waitIdleConnection();

    /**
     * @brief 释放超时的空闲连接
     */
    void checkIdleConnections();

private:
    /**
     * @brief 用于连接池的连接类，
     * 实际上是对ConnectionFactory产生的Connection对象的包装
     */
    class ConnectionImplCP : public Connection
    {
    public:
        /**
         * @brief 构造函数
         *
         * @param pConn ConnectionFactory产生的Connection对象
         */
        ConnectionImplCP( ConnectionPool* pCP, Connection* pConn );

        Statement* createStatement();

        void close();

        int setAutoCommit( bool ac );

        int connect( const std::string& dsn );

    private:
        ~ConnectionImplCP();  // can't delete

    private:
        // ConnectionFactory产生的连接对象
        Connection* m_pConn;
        // 连接池指针
        ConnectionPool* m_pCP;
    };

    /**
     * @brief 内部对每个连接维护的信息
     */
    struct ConnectionInfo
    {
        //ConnectionFactory产生的Connection对象
        Connection* m_pConn;
        //此连接已空闲时间, -1表示正在使用
        int m_nIdleTimes;
    };

private:
    std::string m_DSN;  // data source name
    int m_nMaxIdle;  // max idle connections
    int m_nMaxTotal;  // max connections
    // 空闲连接队列
    std::list<ConnectionInfo> m_idleConnList;
    // 活动连接队列
    std::list<ConnectionInfo> m_activeConnList;
    // 保护两个connection list
    Mutex m_mutexConnList;  // must be declared before m_condIdleConn
    // 等待空闲连接
    Cond m_condIdleConn;
};

#endif  // COMMON_ODBC_CONNECTIONPOOL_H_


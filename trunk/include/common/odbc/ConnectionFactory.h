/*
 * =============================================================================
 *
 *       Filename:  ConnectionFactory.h
 *
 *    Description:  数据库连接工厂类。在createConnection之前应设置相应属性，
 *                  并调用init。
 *
 *        Version:  1.0
 *        Created:  2011-05-07 12:23:46
 *  Last Modified:  2011-05-07 12:23:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#ifndef COMMON_ODBC_CONNECTIONFACTORY_H_
#define COMMON_ODBC_CONNECTIONFACTORY_H_

#include <string>
#include <sys/types.h>
#include <stdint.h>
#include <sql.h>

#include "common/sys/Singleton.h"

class Connection;

/**
 * @brief 数据库连接工厂类
 * 在createConnection之前应设置相应属性，
 * 并调用init。
 */
class ConnectionFactory : public Singleton<ConnectionFactory>
{
    friend class Singleton<ConnectionFactory>;
public:
    ~ConnectionFactory();

    /**
     * @brief 工厂方法，创建一个数据连接对象
     *
     * @return 指向连接对象的指针
     */
    Connection* createConnection() const;  // factory method

    /**
     * @brief 初始化工厂
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    int init();

    /**
     * @brief 设置连接池属性
     *
     * @param pooling
     *      - ODBC_OFF
     *      - ODBC_ONE_PER_DRIVER
     *      - ODBC_ONE_PER_HENV (default)
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    int setConnectionPooling( uint32_t pooling );

    /**
     * @brief 设置ODBC版本
     *
     * @param version
     *      - ODBC_V2
     *      - ODBC_V3 (default)
     *      - ODBC_V3_8
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    int setVersion( uint32_t version );

private:
    // can't new
    ConnectionFactory();
    // can't copy
    ConnectionFactory( const ConnectionFactory& cf );
    ConnectionFactory& operator=( const ConnectionFactory& cf );

    /**
     * @brief 设置Env属性
     *
     * @param attr
     * @param valuePtr
     * @param strLen
     *
     * @return
     *      -1 - failed
     *       0 - success
     */
    int doSetEnvAttr(
        SQLINTEGER attr,
        SQLPOINTER valuePtr,
        SQLINTEGER strLen ) const;

private:
    static SQLHENV m_env;  // handle for ODBC environment

    bool m_bInited;  // 是否已初始化

    // env attrs
    // pooling
    SQLUINTEGER m_pooling;
    // ODBC Version
    SQLUINTEGER m_version;
};


#endif  // COMMON_ODBC_CONNECTIONFACTORY_H_


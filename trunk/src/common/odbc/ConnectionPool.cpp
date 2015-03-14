/*
 * =====================================================================================
 *
 *       Filename:  ConnectionPool.cpp
 *
 *    Description:  连接池实现
 *
 *        Version:  1.0
 *        Created:  04/26/2011 11:51:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */


#include <cstdlib>

#include "common/odbc/ConnectionPool.h"
#include "common/odbc/ConnectionFactory.h"
#include "common/odbc/Statement.h"
#include "common/sys/Lock.h"
#include "common/comm/Error.h"
#include "common/log/log.h"

ConnectionPool::ConnectionPool(
    const std::string& dsn,
    int maxIdle,
    int maxTotal ) :
    m_DSN( dsn ),
    m_nMaxIdle( maxIdle ),
    m_nMaxTotal( maxTotal ),
    m_condIdleConn( m_mutexConnList )
{

    if ( maxIdle <= 0
            || maxTotal <= 0
            || maxIdle > maxTotal )
    {
        m_nMaxIdle = CONN_MAX_IDLE_DEFAULT;
        m_nMaxTotal = CONN_MAX_TOTAL_DEFAULT;
    }
}

ConnectionPool::~ConnectionPool()
{
}

int ConnectionPool::start()
{
    int rt = ConnectionFactory::getInstance()->init();
    if ( -1 == rt )
    {
        ERROR_LOG( "Error occurred while initializing ConnectionFactory." );
        return FAILED;
    }

    for ( int i = 0; i < m_nMaxIdle; ++i )
    {
        Connection* pConn =
            ConnectionFactory::getInstance()->createConnection();
        if ( NULL == pConn )
        {
            ERROR_LOG( "Error occurred while creating a connection." );
            return FAILED;
        }

        rt = pConn->connect( m_DSN );
        if ( -1 == rt )
        {
            ERROR_LOG( "Can't make connection to database." );
            return FAILED;
        }

        ConnectionInfo connInfo;
        connInfo.m_pConn = pConn;
        connInfo.m_nIdleTimes = 0;

        m_idleConnList.push_back( connInfo );
    }

    return SUCCESSFUL;
}

Connection* ConnectionPool::getConnection()
{
    // acquire lock
    Lock lock( m_mutexConnList );

    Connection* pConn;

    // try to get an idle connection
    if ( ( pConn = getIdleConnection() ) != NULL )
    {
        return pConn;
    }

    // no idle connection, try to create a new connection
    if ( ( pConn = newConnection() ) != NULL )
    {
        return pConn;
    }

    // can't create new connection,
    // wait an idle connection
    return waitIdleConnection();

    // release lock
}


Connection* ConnectionPool::getIdleConnection()
{
    Connection* pConn = NULL;

    if ( !m_idleConnList.empty() )  // has idle connection
    {
        ConnectionInfo connInfo = m_idleConnList.front();
        m_idleConnList.pop_front();
        connInfo.m_nIdleTimes = -1;  // set busy
        m_activeConnList.push_back( connInfo );
        pConn = new ConnectionImplCP( this, connInfo.m_pConn );
    }

    return pConn;
}

Connection* ConnectionPool::newConnection()
{
    Connection* pConn = NULL;

    if ( ( m_idleConnList.size() + m_activeConnList.size() )
            < ( size_t )m_nMaxTotal ) // 没有达到最大连接数
    {
        pConn = ConnectionFactory::getInstance()->createConnection();
        int rt = 0;
        if ( pConn != NULL && ( rt = pConn->connect( m_DSN ) ) == 0 )
        {
            ConnectionInfo connInfo;
            connInfo.m_nIdleTimes = -1;
            connInfo.m_pConn = pConn;
            m_activeConnList.push_back( connInfo );
            pConn = new ConnectionImplCP( this, pConn );
        }
        else if ( -1 == rt )
        {
            pConn->close();;
            pConn = NULL;
        }
    }

    return pConn;
}

Connection* ConnectionPool::waitIdleConnection()
{
    // wait for idle connection
    while ( m_idleConnList.empty() )
    {
        if ( m_condIdleConn.wait() )
        {
        }
    }

    ConnectionInfo connInfo = m_idleConnList.front();
    m_idleConnList.pop_front();

    connInfo.m_nIdleTimes = -1;  // set busy
    m_activeConnList.push_back( connInfo );
    Connection* pConn = new ConnectionImplCP( this, connInfo.m_pConn );

    return pConn;
}

void ConnectionPool::releaseToConnectionPool(
    Connection* pConn )
{
    // acquire lock
    Lock lock( m_mutexConnList );

    std::list<ConnectionInfo>::iterator it_active;

    // look up pConn in active list
    for ( it_active = m_activeConnList.begin();
            it_active != m_activeConnList.end();
            ++it_active )
    {
        if ( ( *it_active ).m_pConn == pConn ) // found
        {
            break;
        }
    }

    if ( it_active == m_activeConnList.end() )  // not found
    {
        // close connection
        pConn->close();
    }
    else      // found
    {
        ( *it_active ).m_nIdleTimes = 0;
        m_idleConnList.push_back( ( *it_active ) );
        std::list<ConnectionInfo>::iterator erit = 
            m_activeConnList.erase( it_active );
        if ( erit != m_activeConnList.end() )
        {
        }
        m_condIdleConn.notify();
    }

    // 空间连接数超过限制
    if ( m_idleConnList.size() > ( size_t )m_nMaxIdle )
    {
        checkIdleConnections();
    }

    // release lock
}

void ConnectionPool::checkIdleConnections()
{
    for ( std::list<ConnectionInfo>::iterator
            it = m_idleConnList.begin();
            it != m_idleConnList.end(); )
    {

        if ( ++( ( *it ).m_nIdleTimes ) > CONN_MAX_IDLE_TIMES ) // 超过最大空闲时间
        {

            ( *it ).m_pConn->close(); // close connection
            std::list<ConnectionInfo>::iterator ciit = 
                m_idleConnList.erase( it++ );
            if(ciit == m_idleConnList.end())
            {
            }

            // 检查空闲连接数
            if ( m_idleConnList.size() <= ( size_t )m_nMaxIdle )
            {
                break;
            }
        }
        else
        {
            ++it;
        }
    }
}

ConnectionPool::ConnectionImplCP::ConnectionImplCP(
    ConnectionPool* pCP,
    Connection* pConn )
    : m_pConn( pConn ), m_pCP( pCP )
{
    if ( pCP == NULL || pConn == NULL )
    {
        Logger* logger = LoggerFactory::getInstance()->getRootLogger();
        logger->error( "NULL pointer whilst initializing ConnectionPool::ConnectionImplCP. "
                       "Program will exit." );
        exit( 1 );
    }
}

ConnectionPool::ConnectionImplCP::~ConnectionImplCP()
{
    try
    {
	    m_pCP->releaseToConnectionPool( m_pConn );
	    m_pConn = NULL;
	    m_pCP = NULL;
    }
    catch ( ... )
    {
    }
}

Statement*
ConnectionPool::ConnectionImplCP::createStatement()
{
    return m_pConn->createStatement();
}

void
ConnectionPool::ConnectionImplCP::close()
{
    delete this;
}

int
ConnectionPool::ConnectionImplCP::setAutoCommit( bool ac )
{
    return m_pConn->setAutoCommit( ac );
}

int
ConnectionPool::ConnectionImplCP::connect( const std::string& dsn )
{
    return m_pConn->connect( dsn );
}


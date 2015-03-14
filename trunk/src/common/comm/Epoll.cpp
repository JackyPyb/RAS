#include <time.h>
#include <string.h>
#include <errno.h>

#include "common/comm/Epoll.h"
#include "common/comm/TCPAgent.h"
#include "common/comm/Error.h"
#include "common/comm/AgentManager.h"
#include "common/comm/TaskManager.h"
#include "common/log/log.h"

#define EPOLL_TIMEOUT_LEN 5

Epoll::Epoll( void ):
    m_epollEvents( NULL ),
    m_epollFd( -1 ),
    m_bFinished(false)
{
//    gettimeofday(&m_current, NULL);
}

Epoll::~Epoll( void )
{
    if ( m_epollEvents != NULL )
    {
        delete [] m_epollEvents;
        m_epollEvents = NULL;
    }
}

int Epoll::getEpollFd()const
{
    return m_epollFd;
}

int Epoll::initialize( int fdsize )
{
    m_eventSize = fdsize;
    m_epollFd = epoll_create( m_eventSize );
    if ( m_epollFd < 0 )
    {
        ERROR_LOG( "Epoll:initialize error" );
        return FAILED;
    }
    m_epollEvents = new epoll_event[fdsize];
    memset( m_epollEvents, 0, sizeof( epoll_event )*fdsize );
    return SUCCESSFUL;
}

const struct timeval& Epoll::getCurrent( void ) const
{
    return m_current;
}

int Epoll::doEvent( EpollEvent* ptr,
                    int fd,
                    int op,
                    unsigned int events )
{
    struct epoll_event ev;
    memset( &ev, 0, sizeof( struct epoll_event ) );
    ev.events = events;
    ev.data.ptr = ptr;
    if ( epoll_ctl( m_epollFd, op, fd, &ev ) < 0 )
    {
        ERROR_LOG( "Epoll:doEvent epoll_ctl() error %s",strerror(errno) );
        return FAILED;
    }
    return SUCCESSFUL;
}


void Epoll::run( void )
{

    int nfds = 0;

    EpollEvent* event = NULL;

    for ( ;; )
    {
        if ( ( nfds = epoll_wait(
                          m_epollFd, m_epollEvents,   m_eventSize, EPOLL_TIMEOUT_LEN ) ) < 0 )
        {

//          printf("epollfd: %d, m_eventSize: %d, timeout:%d\n",m_epollFd, m_eventSize, EPOLL_TIMEOUT_LEN);
            if ( EINTR  == errno )
            {
                continue;
            }
            else
            {
                ERROR_LOG( "Epoll:epoll_wait" );
            }
        }

        if ( gettimeofday( &m_current, NULL ) < 0 )
        {
            ERROR_LOG( "Epoll:gettimeofday" );
        }

//      unsigned long long t = 100000 * m_current.tv_sec + m_current.tv_usec;
        //  printf("epoll: time: %d\n",t/1000);

        for ( int i = 0; i < nfds; i++ )
        {

            event = ( EpollEvent* )m_epollEvents[i].data.ptr;
            if ( NULL == event )
            {
                ERROR_LOG( "Epoll::event == NULL" );
                continue;
            }
            //INFO_LOG("id: %u", event->getHandlerID());
            Agent* agent = AgentManager::getInstance()->get(
                event->getHandlerID());
            if ( NULL == agent )
            {
                ERROR_LOG( "Epoll::agent == NULL" );
                continue;
            }
            if ( m_epollEvents[i].events & EPOLLERR
                    || m_epollEvents[i].events & EPOLLHUP )
            {
                if ( agent->getState() == CONNECTING ) //for the connect
                {
                    if ( agent->getErrno() == EISCONN )
                    {
                        agent->setState( CONNECTED );
                        if ( agent->connectAfter( true ) < 0 )
                        {
                            ( AgentManager::getInstance() )->recycle( agent->getID());
                            continue;
                        }
                        agent->resetConnect();
                        continue;
                    }
                    else
                    {
                        if ( agent->allowReconnect() )
                        {
                            SocketAddress addr;
                            if ( SUCCESSFUL == ( agent->getOppAddr( addr ) ) )
                            {
                                if ( agent->connect( addr ) < 0 )
                                {
                                }
                            }
                            else
                            {
                                ERROR_LOG( "OppAddr error" );
                                ( AgentManager::getInstance() )->recycle( agent->getID());
                            }
                            continue;
                        }
                        else
                        {
                            agent->resetConnect();
                            if ( agent->connectAfter( false ) < 0 )
                            {
                                ERROR_LOG( "connect error" );
                                ( AgentManager::getInstance() )->recycle( agent->getID());
                                continue;
                            }
                            else
                            {
                                continue;
                            }
                        }
                    }
                }
                else
                {
                    if ( agent->recvData() < 0 )
                    {
                        ( AgentManager::getInstance() )->recycle( agent->getID());
                        continue;
                    }
                }
            }
            if ( m_epollEvents[i].events & EPOLLOUT )
            {
                if ( CONNECTED == ( agent->getState() ) )
                {
                    if ( agent->sendData() < 0 )
                    {
                        ( AgentManager::getInstance() )->recycle( agent->getID());
                        continue;
                    }
                }
                else
                {
                    agent->setState( CONNECTED );
                    if ( agent->connectAfter( true ) < 0 )
                    {
                        ( AgentManager::getInstance() )->recycle( agent->getID());
                    }
                    continue;
                }
            }
            if ( m_epollEvents[i].events & EPOLLIN )
            {
                if ( agent->recvData() < 0 )
                {
                    ( AgentManager::getInstance() )->recycle( agent->getID());
                    continue;
                }
            }
            
        }
        if ( doTask() < 0 )
        {
            INFO_LOG( "Deal With Task Error" );
        }
        if ( m_bFinished )
        {
            break;
        }
    }
}

void
Epoll::finish()
{
    m_bFinished = true;
}

int Epoll::doTask( void )
{
    if ( m_bFinished )
    {
        AgentManager::getInstance()->clearAllAgent();
        TaskManager::getInstance()->clearAllTask();
    }
    AgentManager::getInstance()->recycleAll();
    TaskManager::getInstance()->recycleAll();
    return SUCCESSFUL;
}

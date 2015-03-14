#include <sys/epoll.h>

#include "common/comm/EpollEvent.h"
#include "common/comm/Epoll.h"
#include "common/comm/Error.h"
#include "common/log/log.h"

//#define EDGEEVENTS                   (EPOLLET|EPOLLOUT|EPOLLIN | EPOLLERR | EPOLLHUP)
//#define EDGEREVENT                   (EPOLLET|EPOLLIN | EPOLLERR | EPOLLHUP)
#define LEVELREVENT                  EPOLLIN
#define LEVELWEVENT                  EPOLLOUT
#define LEVELEVENTS                  (EPOLLIN |EPOLLOUT| EPOLLERR | EPOLLHUP)
#define EVENTADD                     EPOLL_CTL_ADD
#define EVENTDEL                     EPOLL_CTL_DEL
#define EVENTCHANGE                  EPOLL_CTL_MOD

int EpollEvent::closeRevent( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    if ( m_epollEvent.rlock++ )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.read = false;
    m_epollEvent.event = m_epollEvent.event &~LEVELREVENT;
    if ( g_pEpoll->doEvent( this, m_fd, EVENTCHANGE, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::closeRevent" );
        return FAILED;
    }
    return SUCCESSFUL;
}

int EpollEvent::openRevent( void )
{

    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    if ( ( m_epollEvent.rlock && --m_epollEvent.rlock )
            || m_epollEvent.read )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.read = true;
    m_epollEvent.event = m_epollEvent.event | LEVELREVENT;
    if ( g_pEpoll->doEvent( this, m_fd, \
                           EVENTCHANGE, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::OpenRevent" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int EpollEvent::closeWevent( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    if ( m_epollEvent.wlock++ )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.write = false;
    m_epollEvent.event = m_epollEvent.event &~LEVELWEVENT;
    if ( g_pEpoll->doEvent( this, m_fd, \
                           EVENTCHANGE, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::closeWevent" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int EpollEvent::openWevent( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    if ( !( m_epollEvent.write ) )
    {
        if ( ( m_epollEvent.wlock && --m_epollEvent.wlock )
                || m_epollEvent.write )
        {
            return SUCCESSFUL;
        }

        m_epollEvent.write = true;
        m_epollEvent.event = m_epollEvent.event | LEVELWEVENT;
        if ( g_pEpoll->doEvent( this, m_fd, \
                               EVENTCHANGE, m_epollEvent.event ) < 0 )
        {
            ERROR_LOG( "EpollEvent::OpenWevent" );
            return FAILED;
        }
    }
    return SUCCESSFUL;
}

int EpollEvent::registerREvent( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.event = LEVELREVENT;
    m_epollEvent.read = true;
    m_epollEvent.write = false;


    if ( g_pEpoll->doEvent( this, m_fd, \
                           EVENTADD, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::registerREvent" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int EpollEvent::registerWEvent( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.event = LEVELWEVENT;
    m_epollEvent.read = false;
    m_epollEvent.write = true;


    if ( g_pEpoll->doEvent( this, m_fd, \
                           EVENTADD, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::registerWEvent" );
        return FAILED;
    }
    return SUCCESSFUL;
}

int EpollEvent::registerRWEvents( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.event = LEVELEVENTS;
    m_epollEvent.read = true;
    m_epollEvent.write = true;


    if ( g_pEpoll->doEvent( this, m_fd, \
                           EVENTADD, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::registerRWEvent" );
        return FAILED;
    }

    return SUCCESSFUL;
}

int EpollEvent::unregisterRWEvents( void )
{
    if ( -1 == m_fd )
    {
        return SUCCESSFUL;
    }

    m_epollEvent.event = LEVELEVENTS;
    m_epollEvent.read = false;
    m_epollEvent.write = false;


    if ( g_pEpoll->doEvent( this, m_fd, \
                           EVENTDEL, m_epollEvent.event ) < 0 )
    {
        ERROR_LOG( "EpollEvent::UnregisterRWEvent" );
        return FAILED;
    }

    return SUCCESSFUL;
}


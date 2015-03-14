#ifndef __EPOLLEVENT_H__
#define __EPOLLEVENT_H__

#include <stdio.h>
#include <stdint.h>

class EpollEvent
{
    typedef struct
    {
        unsigned int event;
        unsigned int rlock;
        unsigned int wlock;
        bool            read;
        bool        write;
    } event_st;
public:
    EpollEvent()
    {
        m_epollEvent.event = 0;
        m_epollEvent.rlock = 0;
        m_epollEvent.wlock = 0;
        m_handlerId = 0;
        m_fd = -1;

    }
    ~EpollEvent( void )
    {
    }

    /*
    **set the epoll fd
    */
    void setFd( int fd )
    {
        m_fd = fd;
        m_epollEvent.event = 0;
        m_epollEvent.rlock = 0;
        m_epollEvent.wlock = 0;
    }
    /*
    **set handler
    */
    void setHandlerID( uint32_t handlerid )
    {
        m_handlerId = handlerid;
    }

    uint32_t getHandlerID( void )
    {
        return m_handlerId;
    }

    /*open the read event on sock
    **retun suc(0) error(-1)
    */
    int openRevent( void );
    /*open the write event on  sock
    **return suc(0) error(-1)
    */
    int openWevent( void );
    /*close the read event on sock
    **return suc(0) error(-1)
    */
    int closeRevent( void );
    /*close the write event on sock
    **return suc(0) error(-1)
    */
    int closeWevent( void );
    /*register the read event on sock
    **return suc(0) error(-1)
    */
    int registerREvent( void );
    /*register the write event on sock
    **return suc(0) error(-1)
    */
    int registerWEvent( void );
    /*register the read and write event on sock
    **return suc(0) error(-1)
    */
    int registerRWEvents( void );

    int unregisterRWEvents( void );

protected:
    event_st       m_epollEvent;
    uint32_t     m_handlerId;
    int            m_fd;
};

#endif


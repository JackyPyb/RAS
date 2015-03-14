#ifndef __EPOLLEVENTSCHEDULER_H__
#define __EPOLLEVENTSCHEDULER_H__

#include <sys/epoll.h>
#include <sys/time.h>
#include <list>

#include "common/comm/Error.h"

class EpollEvent;

class Epoll
{
public:
    Epoll();
    virtual ~Epoll( void );
    int getEpollFd()const;
    /*initialize the epoll fds
    **return suc(0) fail(-1)
    */
    int initialize( int );

    /*
    **get current system time based on millisecond
    */
    const struct timeval& getCurrent( void ) const;

    /*add or change event on fd
    **return suc(0) fail (-1)
        */
    int doEvent( EpollEvent* ptr, int fd, int op, unsigned int events );

    /*epoll loop
      **
      */
    void run( void );
	void finish();
    virtual int doTask( void );

private:

    struct epoll_event*   m_epollEvents;
    int                   m_epollFd;
    int          m_eventSize;
    struct timeval        m_current;
    std::list<int>  m_list;

	bool m_bFinished;
};
extern Epoll *g_pEpoll;

#endif


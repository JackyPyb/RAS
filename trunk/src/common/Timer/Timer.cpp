// =====================================================================================
//
//       Filename:  Timer.cpp
//
//    Description:
//
//        Version:  1.4
//        Created:  04/03/2010 09:14:07 AM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Peng Xuyu (Elyxier), pxy1987@gmail.com
//        Company:  NDSL UESTC
//
// =====================================================================================


#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <algorithm>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>

#include "common/Timer/Timer.h"
#include "common/log/log.h"

static int
SetNonblock( int fd )
{
    int val = fcntl( fd, F_GETFL, 0 );
    if ( val < 0 )
    {
        ERROR_LOG( "SetNonblock::fcntl" );
        return TIMER_FAIL;
    }
    if ( ( val = fcntl( fd, F_SETFL, val | O_NONBLOCK ) ) < 0 )
    {
        ERROR_LOG( "SetNonblock::fcntl" );
        return TIMER_FAIL;
    }
    return TIMER_SUCCESS;
}


static void
TimerSigAlrm( int )           //operated only by Timer Thread
{
    g_pTimerManager->checkTimeOut();
}

static void*
TimerThread( void* )
{
    sigset_t      sset;
    Timer*        p;
    struct itimerval    val;

    if ( sigemptyset( &sset ) || sigaddset( &sset, SIGALRM ) )
    {
        pthread_exit( ( void* )0 );
    }
    while ( pthread_sigmask( SIG_UNBLOCK, &sset, NULL ) < 0 ) {}

    val.it_value.tv_sec = 1;
    val.it_value.tv_usec = 0;
    val.it_interval.tv_sec = 1;
    val.it_interval.tv_usec = 0;

    while ( setitimer( ITIMER_REAL, &val, NULL ) < 0 ) {}


    //timermanager loops to read Timer objs from message queue 1,and treat the clock events
    for ( ;; )
    {
        //TimerManager reads message queue 1
        p = g_pTimerManager->recvTimer();
        if ( p == NULL )
        {
            continue;
        }
        else
        {
            if ( g_pTimerManager->insertTimer( p ) == TIMER_1ST_POS )
            {
                g_pTimerManager->checkTimeOut();
            }
        }
    }
}




//========================== Timer ========================================================




//constructor
Timer::Timer( unsigned int mtime, long pri ) :
    m_bDetached( false ),
    m_bDone( false ),
    m_changing( 0 ),
    m_tid( pthread_self() ),
    m_pri( pri ),
    m_timeOutLen( mtime ),
    m_expiredTime( mtime + ( unsigned int )( time( NULL ) ) )
{
}

Timer::Timer( void ) :
    m_bDetached( false ),
    m_bDone( false ),
    m_changing( 0 ),
    m_tid( pthread_self() ),
    m_pri( 100 ),
    m_timeOutLen( 1000000000 ),
    m_expiredTime( 1000000000 )
{
}


//insert the timer obj into message queue 1
int
Timer::attachTimer()
{
    m_bDone = false;
    m_bDetached = false;
    if ( TIMER_FAIL 
        == g_pTimerManager->registerThread( pthread_self() ) )
    {
        ERROR_LOG( "Timer::attackTimer:registerThread error." );
        return FAILED;
    }
    g_pTimerManager->MTSendTimer( this, TIMER_CMD_ADD );
    return TIMER_SUCCESS;
}

//remove the timer obj from message queue 1
int
Timer::detachTimer()
{
    if ( m_bDone )
    {
        delete this;
        return SUCCESSFUL;
    }
    m_bDetached = true;
    g_pTimerManager->MTSendTimer( this, TIMER_CMD_DEL );
    return SUCCESSFUL;
}

bool
Timer::detached() const
{
    return m_bDetached;
}

int
Timer::changing() const
{
    return m_changing;
}

void
Timer::cancelChange()
{
    --m_changing;
}

int
Timer::updateExpiredTime( unsigned int newLen )
{
    m_expiredTime = newLen + ( unsigned int )( time( NULL ) );
    if ( !m_bDone )
    {
        ++m_changing;
        g_pTimerManager->MTSendTimer( this, TIMER_CMD_CHANGE );

    }
    else
    {
        m_bDone = false;
        g_pTimerManager->MTSendTimer( this, TIMER_CMD_ADD );
    }
    return TIMER_SUCCESS;
}

//get the expiredtime of the Timer obj
unsigned int
Timer::getExpiredTime() const
{
    return m_expiredTime;
}

//set the expiredtime of the Timer obj
int
Timer::setExpiredTime( unsigned int len )
{
    m_expiredTime = len + ( unsigned int )time( NULL );
    m_timeOutLen = len;
    return 1;
}

//get the Timer object's priority
long
Timer::getPriority() const
{
    return m_pri;
}

//set the Timer object's priority
void
Timer::setPriority( long pri )
{
    m_pri = pri;
}

//get the process ID of the Timer obj
pthread_t
Timer::getTid() const
{
    return m_tid;
}





//======================== TimerManager  ==========================================================




//constructor
TimerManager::TimerManager()
{
    while ( pipe( m_msgPipe1 ) < 0 ) {}
}

//destructor
TimerManager::~TimerManager()
{
    try
    {
        std::map<pthread_t, int>::iterator mapIt;
        std::multiset<Timer*>::iterator   it;
    
        for ( it = m_timerList.begin(); it != m_timerList.end(); ++it )
        {
            delete ( *it );
        }
        m_timerList.clear();
    
        close( m_msgPipe1[0] );
        close( m_msgPipe1[1] );
    
        for ( mapIt = m_msgRPipeList.begin(); 
            mapIt != m_msgRPipeList.end(); ++mapIt )
        {
            close( mapIt->second );
        }
        m_msgRPipeList.clear();
        for ( mapIt = m_msgWPipeList.begin(); 
            mapIt != m_msgWPipeList.end(); ++mapIt )
        {
            close( mapIt->second );
        }
        m_msgWPipeList.clear();
    }
    catch ( ... )
    {
    }
}

//register thread in TimerManager, which will allocate a pipe for back sending if it's not registered yet
int
TimerManager::registerThread( pthread_t tid )
{
    int p[2];

    if ( !( m_msgRPipeList.count( tid ) == 0 
        && m_msgRPipeList.count( pthread_self() ) == 0 ) )
    {
        return TIMER_SUCCESS;
    }
    if ( pipe( p ) < 0 )
    {
        ERROR_LOG( "TimerManager::registerThread::pipe" );
        return TIMER_FAIL;
    }
    if ( FAILED == SetNonblock( p[0] ) )
    {
        ERROR_LOG( "TimerManager::registerThread failed." );
        return FAILED;
    }
    m_msgRPipeList[tid] = p[0];
    m_msgWPipeList[tid] = p[1];

    m_epollEvent.setFd( p[0] );
    m_epollEvent.setHandlerID( getID() );
    if ( FAILED == m_epollEvent.registerREvent() )
    {
        ERROR_LOG( "TimerManager::registerThread failed." );
        return FAILED;
    }

    return TIMER_SUCCESS;
}

//initialize TimerManager
int
TimerManager::init()
{
    pthread_t    tid = 0;
    struct sigaction act;
    sigset_t             theSet;
    
    int ret = sigemptyset( &theSet );
    if ( ret < 0 )
    {
    }
    ret = sigaddset( &theSet, SIGALRM );
    if ( ret < 0 )
    {
    }
    if ( sigprocmask( SIG_BLOCK, &theSet, NULL ) < 0 )
    {
        ERROR_LOG( "TimerManager::Init:sigprocmask" );
        return TIMER_FAIL;
    }

    bzero( &act, sizeof( act ) );
    ret = sigemptyset( &( act.sa_mask ) );
    if ( ret < 0 )
    {
    }
    act.sa_handler = TimerSigAlrm;
    if ( sigaction( SIGALRM, &act, NULL ) < 0 )
    {
        ERROR_LOG( "TimerManager::Init::sigaction" );
        return TIMER_FAIL;
    }

    if ( SUCCESSFUL 
        != pthread_create( &tid, NULL, TimerThread, NULL ) )
    {
        ERROR_LOG( "TimerManager::init:pthread_create error." );
        return FAILED;
    }
    return TIMER_SUCCESS;
}

//insert a Timer obj into the timerlist
int
TimerManager::insertTimer( Timer* p )
{
    blockSIGALRM();
    std::multiset<Timer*, LessTimer>::iterator tlit = m_timerList.insert( p );
    if ( tlit != m_timerList.end() )
    {
    }
    if ( *( m_timerList.begin() ) == p )
    {
        unblockSIGALRM();
        return TIMER_1ST_POS;
    }
    else
    {
        unblockSIGALRM();
        return TIMER_NOT_1ST_POS;
    }
}

//remove a Timer obj from the timerlist
int
TimerManager::removeTimer( const Timer* p )
{
    multiset<Timer*, LessTimer>::iterator it;

    blockSIGALRM();
    it = m_timerList.begin();
    while ( it != m_timerList.end() )
    {
        if ( *it == p )
        {
            m_timerList.erase( it );
            unblockSIGALRM();
            return TIMER_FOUND;
        }
        ++it;
    }
    unblockSIGALRM();
    return TIMER_NOT_FOUND;
}

//check whether any timer is timeout.and execute function doAction() if it's expired
void
TimerManager::checkTimeOut( void )
{
    unsigned int Tm = ( unsigned int )time( NULL );
    map<pthread_t , multiset<Timer*, LessPri> > l_TidList;
    multiset<Timer*, LessPri> l_ExpiredTimerList; // not used? sw

    map<pthread_t , multiset<Timer*, LessPri> >::iterator listit;
    multiset<Timer*, LessPri>::iterator setit;                        //iterator for traversing
    multiset<Timer*, LessTimer>::iterator it = m_timerList.begin();   //iterator for traversing the timer list m_timerList

    if ( getLength() <= 0 )
    {
        return;
    }

    //if it is timeout,erase the timer from the timerlist and alert the main thread,then send it back
    while ( ( it != m_timerList.end() ) &&  ( *it )->getExpiredTime() <= Tm )
    {
        l_TidList[( *it ) -> getTid()].insert( *it );
        m_timerList.erase( it );
        it = m_timerList.begin();
        Tm = ( unsigned int )time( NULL );
    }
    for ( listit = l_TidList.begin() ; listit != l_TidList.end() ; ++listit )
    {
        for ( setit = ( listit->second ).begin() ; 
            setit != ( listit->second ).end() ; ++setit )
        {
            int ret = sendTimer( *setit );
            if ( ret < 0 )
            {
            }
        }
    }

    return;
}





//TimerManager reads one Timer from main thread through message queue 1
Timer*
TimerManager::recvTimer()
{
    ssize_t           n;
    int              result;
    Timer*           p;
    TimerMsg_t       theMsg;

    result = -1;
    bzero( &theMsg, sizeof( TimerMsg_t ) );
    for ( ;; )                                                  //loop until msg is read successfully
    {
        n = read( m_msgPipe1[0], &theMsg, sizeof( TimerMsg_t ) );
        if ( n < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else
            {
                ERROR_LOG( "TimerManager::recvTimer::read" );
                continue;
            }
        }
        else
        {
            if ( n != sizeof( TimerMsg_t ) )
            {
                return NULL;
            }
            break;
        }
    }

    p = ( Timer* )( theMsg.ptr );
    if ( theMsg.cmd == TIMER_CMD_DEL )                              //a deleting message
    {
        if ( removeTimer( p ) == TIMER_FOUND )                    //the obj's in TimerList
        {
            checkTimeOut();
            int ret = sendTimer( p );                                       //send it back to MT
            if ( ret < 0 )
            {
            }
        }
        return NULL;
    }
    else if ( theMsg.cmd == TIMER_CMD_CHANGE )
    {
        if ( removeTimer( p ) == TIMER_FOUND )
        {
            p->cancelChange();
            result = insertTimer( p );
        }
        else
        {
            result = insertTimer( p );
        }
        if ( result == TIMER_1ST_POS )
        {
            checkTimeOut();
        }
        return NULL;
    }

    return p;
}

//TimerManager sends one expired Timer to main thread through message queue 2
int
TimerManager::sendTimer( Timer* p )
{
    TimerMsg_t         theMsg;

    bzero( &theMsg, sizeof( TimerMsg_t ) );
    theMsg.mpri = p->getPriority();
    theMsg.ptr = p;
    theMsg.cmd = TIMER_CMD_NULL;
    for ( ;; )
    {
        if ( write( m_msgWPipeList[p->getTid()], 
            ( const void* )&theMsg, sizeof( TimerMsg_t ) ) < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else
            {
                ERROR_LOG( "TimerManager::sendTimer:write" );
                return TIMER_FAIL;
            }
        }
        else
        {
            break;
        }
    }
    return TIMER_SUCCESS;
}


//main thread reads one expired Timer from message queue 2
Timer*
TimerManager::MTRecvTimer()
{
    int        nbytes;
    Timer*     p;
    TimerMsg_t theMsg;

    bzero( &theMsg, sizeof( TimerMsg_t ) );
    for ( ;; )
    {
        nbytes = read( m_msgRPipeList[pthread_self()], 
            &theMsg, sizeof( TimerMsg_t ) );
        if ( nbytes < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else if ( errno == EAGAIN )
            {
                return NULL;
            }
            else
            {
                ERROR_LOG( "TimerManager::MTRecvTimer:read" );
                return NULL;
            }
        }
        else
        {
            if ( ( size_t )nbytes != sizeof( TimerMsg_t ) )
            {
                continue;
            }
            break;
        }
    }
    p = ( Timer* )( theMsg.ptr );
    return p;
}

//main thread sends one Timer to TimerManager through message queue 1
void
TimerManager::MTSendTimer(
    Timer* p, int cmd )
{
    TimerMsg_t theMsg;
    int n;

    bzero( &theMsg, sizeof( TimerMsg_t ) );
    theMsg.mpri = p->getPriority();
    theMsg.cmd = cmd;
    theMsg.ptr = p;

    for ( ;; )
    {
        n = write( m_msgPipe1[1], 
            ( const void* )&theMsg, sizeof( TimerMsg_t ) );
        if ( n < 0 )
        {
            if ( errno == EINTR )
            {
                continue;
            }
            else
            {
                ERROR_LOG( "TimerManager::MTSendTimer:write" );
                continue;
            }
        }


        else
        {
            break;
        }
    }
}

int
TimerManager::getLength() const
{
    return m_timerList.size();
}

int
TimerManager::recvData()
{
    Timer* p;
    for ( ;; )
    {
        p = g_pTimerManager->MTRecvTimer();
        if ( p == NULL )
        {
            break;
        }
        if ( p->changing() )  // 如果Timer正在被改变，忽略此次事件
        {
            p->cancelChange();
            continue;
        }
        if ( p->detached() )
        {
            delete p;
            p = NULL;
            continue;
        }

        p->setDone();
        if ( FAILED == p->doAction() )
        {
            ERROR_LOG( "Timer::doAction error." );
            return SUCCESSFUL;
        }
    }

    return SUCCESSFUL; // ?? niexw
}

bool
Timer::isDone() const
{
    return m_bDone;
}

void Timer::setDone()
{
    m_bDone = true;
}

void
TimerManager :: blockSIGALRM( void )
{
    sigset_t             theSet;
    
    int ret = sigemptyset( &theSet );
    if ( ret < 0 )
    {
    }
    ret = sigaddset( &theSet, SIGALRM );
    if ( ret < 0 )
    {
    }

    if ( pthread_sigmask( SIG_BLOCK, &theSet, NULL ) < 0 )
    {
        ERROR_LOG( "pthread_sigmask error" );
    }
}

void
TimerManager :: unblockSIGALRM( void ) const
{
    sigset_t             theSet;
    
    int ret = sigemptyset( &theSet );
    if ( ret < 0 )
    {
    }
    ret = sigaddset( &theSet, SIGALRM );
    if ( ret < 0 )
    {
    }

    if ( pthread_sigmask( SIG_UNBLOCK, &theSet, NULL ) < 0 )
    {
        ERROR_LOG( "pthread_sigmask error" );
    }
}


/*
 * =============================================================
 *
 *    Filename:  Timer.h
 *
 * Description:  定时器实现
 *
 * =============================================================
 */

#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <map>
#include <set>
#include <sys/types.h>
#include <sys/epoll.h>

#include "common/comm/Agent.h"
#include "common/comm/EpollEvent.h"
#include "common/comm/Error.h"

using namespace std;

#define    TIMER_MSG_SIZE       20
#define    TIMER_SUCCESS        SUCCESSFUL
#define    TIMER_FAIL           FAILED
#define    TIMER_CMD_ADD        1
#define    TIMER_CMD_DEL        2
#define    TIMER_CMD_CHANGE     3
#define    TIMER_CMD_NULL       0
#define    TIMER_FILEPATH       "./"
#define    TIMER_1ST_POS        1
#define    TIMER_NOT_1ST_POS    0
#define    TIMER_FOUND          1
#define    TIMER_NOT_FOUND      0
#define    TIMER_MAXPIPES       100

/**
 * @brief 定时器
 * 通过继承此类，并重写doAction方法来实现不同的定时任务
 *
 */
class Timer
{
    friend class TimerManager;
public:
    Timer( void );
    Timer( unsigned int, long pri = 1 );

    /**
     * @brief 定时器到期后的动作
     *
     * @return
     */
    virtual int doAction( void )
    {
        return SUCCESSFUL;
    }

    /**
     * @brief 更新到期时间
     *
     * @param int 到期时间，相对时间
     *
     * @return
     */
    virtual int updateExpiredTime( unsigned int );

    /**
     * @brief 清册到TimerManager
     *
     * @param Epoll
     *
     * @return
     */
    int attachTimer();

    /**
     * @brief 从TimerManager中删除
     *
     * @return
     */
    int detachTimer( void );

    /**
     * @brief 获取到期时间，绝对时间
     *
     * @return
     */
    unsigned int getExpiredTime() const;

    /**
     * @brief 设置到期时间，相对时间
     *
     * @param int 到期时间
     *
     * @return
     */
    int setExpiredTime( unsigned int );

    /**
     * @brief 获取此Timer优先级
     *
     * @return
     */
    long getPriority() const;

    /**
     * @brief 设置优先级,
     * 优先级决定同一线程下发的Timer到期时的处理顺序
     *
     * @param long 优先级
     */
    void setPriority( long );

    /**
     * @brief 创建此Timer的线程ID
     *
     * @return
     */
    pthread_t getTid() const;

    /**
     * @brief Timer是否已经被detach
     *
     * @return
     */
    bool detached( void ) const;

    /**
     * @brief Timer是否正在被改变
     *
     * @return
     */
    int changing( void ) const;

    /**
     * @brief m_Changeing计数减1，
     * Timer到期事件被忽略一次
     */
    void cancelChange();

    /**
     * @brief Timer是否已完成，
     * 即已到期
     *
     * @return
     */
    bool isDone() const;

    /**
     * @brief 设置Timer状态为已完成
     */
    void setDone();

protected:
    virtual ~Timer() {}

private:
    bool m_bDetached; //whether this Timers obj has been detached
    bool m_bDone;

    // Timer更新到期时间的计数
    // 如果Timer正在被改变（m_Changing不为0），
    // 则此时Timer到期事件应该被忽略
    int m_changing;

    pthread_t m_tid;  //the thread id of the thread that created this Timer obj
    long m_pri;  //the priority
    unsigned int m_timeOutLen;  //time out length, 相对时间
    unsigned int m_expiredTime;  //timer will be expired at this ime，绝对时间
};

//the rule for sorting of multiset
struct LessTimer : public std::binary_function<Timer*, Timer*, bool>
{
    bool operator()( Timer* leftVal, Timer* rightVal ) const
    {
        return ( leftVal->getExpiredTime() < rightVal->getExpiredTime() );
    }
};

struct LessPri : public std::binary_function<Timer*, Timer*, bool>
{
    bool operator()( Timer* leftVal, Timer* rightVal ) const
    {
        return ( leftVal->getPriority() < rightVal->getPriority() );
    }
};


/**
 * @brief TimerManager用一个线程来维护所有Timer，
 * Timer到期后，TimerManger将其写回相应主线程
 */
class TimerManager : public Agent
{
public:
    TimerManager();
    virtual ~TimerManager();

    /**
     * @brief TimerManger初始化，
     * 主要进行SIGALRM信号处理设置
     *
     * @return
     */
    int init();                   //initialize the TimerThread

    /**
     * @brief 将某线程注册到TimerManager，
     * 这将导致在TimerManager线程和注册线程之间建立一根管道，
     * 并且此管道的读端注册到指定Epoll，TimerMnager可以通过
     * 此管道向线程回写Timer到期信息
     *
     * @param pthread_t 注册线程
     * @param Epoll 注册Epoll
     *
     * @return
     */
    int registerThread( pthread_t );       //

    /**
     * @brief 从Timer list移除Timer
     *
     * @param Timer
     *
     * @return
     */
    int removeTimer( const Timer* );    //remove the address of a Timer obj from m_timerList

    /**
     * @brief 插入Timer到Timer list,
     * 按到期时间（绝对时间）排序
     *
     * @param Timer
     *
     * @return
     */
    int insertTimer( Timer* );    //insert the address of a Timer obj from m_timerList

    /**
     * @brief 检查是否有Timer到期，
     * 如果有将其写回相应下发线程
     */
    void checkTimeOut( void );     //check m_timerList and remove all timed out members

    /**
     * @brief TimerManger线程读取上层下发的Timer指令
     *
     * @return
     */
    Timer* recvTimer();          //TimerManager reads message queue 1

    /**
     * @brief 将到期Timer写回注册线程
     *
     * @param Timer
     *
     * @return
     */
    int sendTimer( Timer* );      //TimerManager writes message queue 2

    /**
     * @brief 注册线程从TimerManager接收一个到期Timer
     *
     * @return
     */
    Timer* MTRecvTimer();        //main thread read message queue 2

    /**
     * @brief 主线程下发Timer指令
     *
     * @param Timer 欲操作的Timer
     * @param int Timer操作指令
     */
    void MTSendTimer( Timer*, int cmd = TIMER_CMD_ADD ); //main thread write message queue 1

    /**
     * @brief 当前Timer个数
     *
     * @return
     */
    int getLength() const;              //return the length of m_timerList

    /**
     * @brief 回写管道读事件到来，主线程接收过期Timer，
     * 执行doAction
     *
     * @return
     */
    int recvData();

    /**
     * @brief not used
     *
     * @return
     */
    int sendData()
    {
        return SUCCESSFUL;
    }

private:
    int        m_msgPipe1[2];            //for TimerManager receiving Timers from main thread

    EpollEvent m_epollEvent;

    // 为每个注册线程维护一根回写管道
    std::map<pthread_t, int> m_msgRPipeList;
    std::map<pthread_t, int> m_msgWPipeList;

    std::multiset<Timer*, LessTimer> m_timerList;

    void blockSIGALRM( void );

    void unblockSIGALRM( void ) const;
};

//the format of data structure that be sent and received in the tow Message Queues
typedef struct
{
    long       mpri;            //priority of Timer obj
    Timer*     ptr;
    int        cmd;
} TimerMsg_t;


extern TimerManager* g_pTimerManager;

#endif  // COMMON_TIMER_H


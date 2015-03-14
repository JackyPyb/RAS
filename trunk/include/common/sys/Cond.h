/*
 * =====================================================================================
 *
 *       Filename:  Cond.h
 *
 *    Description:  条件变量封装
 *
 *        Version:  1.0
 *        Created:  04/26/2011 09:23:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#ifndef COMMON_SYS_COND_H_
#define COMMON_SYS_COND_H_

#include <pthread.h>
#include <cerrno>

class Mutex;
class Time;
/**
 * @brief linux线程条件变量
 */
class Cond
{
public:
    Cond( const Mutex& mutex );
    ~Cond();

    /**
     * @brief 唤醒等待线程
     */
    void notify() const;

    /**
     * @brief 唤醒所有等待线程
     */
    void notifyAll() const;

    /**
     * @brief 线程阻塞等待条件
     *
     * @return
     */
    bool wait() const;

    /**
     * @brief 线程阻塞等待，条件发生或超时返回
     *
     * @param timeout 超时时间
     *
     * @return
     */
    bool timedWait( const Time& timeout ) const;

private:
    mutable pthread_cond_t m_cond;
    const Mutex& m_mutex;
};

#endif  // COMMON_SYS_COND_H_


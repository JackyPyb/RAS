#ifndef _THREADPOOLWORKITEM_H_
#define _THREADPOOLWORKITEM_H_

#include <stdint.h>

#include "common/sys/ThreadPoolDispatcher.h"

/**
 * @brief 线程池任务类接口
 */
class ThreadPoolWorkItem
{
public:
    ThreadPoolWorkItem() {}
    virtual ~ThreadPoolWorkItem() {}

    /**
     * @brief 在执行具体业务前的公共操作
     *
     * @return
     */
    virtual int preProcess()
    {
        return SUCCESSFUL;
    }

    /**
     * @brief 实现此方法以执行具体业务
     *
     * @return
     */
    virtual int process() = 0;

    /**
     * @brief 执行具体业务后的公共操作
     *
     * @return
     */
    virtual int postProcess()
    {
        g_pDispatcher->postResult( this );
        return SUCCESSFUL;
    }

    virtual uint64_t getTaskID()
    {
        return m_taskID;
    }

    virtual void setTaskID( uint64_t id )
    {
        m_taskID = id;
        return;
    }

private:
    uint64_t m_taskID;
};

#endif // _THREADPOOLWORKITEM_H_

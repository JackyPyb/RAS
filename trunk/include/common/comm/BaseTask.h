/*
 * =====================================================================================
 *
 *       Filename:  BaseTask.h
 *
 *    Description:  the virtual base class of all task class
 *
 *        Version:  1.0
 *        Created:  2010年03月26日 14时56分50秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  ptz, ptz_007@uestc.edu.cn
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef _BASETASK_H_
#define _BASETASK_H_

#include <stdint.h>

#include "common/comm/Error.h"

struct BaseRequest;
struct InReq;
class ThreadPoolWorkItem;

class BaseTask
{
public:
    BaseTask() {}
    virtual ~BaseTask() {}

	virtual int goNext() = 0;
	
    virtual int recvReq( BaseRequest* )
	{
        return SUCCESSFUL;
    }

    virtual void readBack( InReq& ) {}
    virtual void writeBack( bool ) {}

    //add for TaskManager
    virtual void setID( uint64_t id )
    {
        m_ID = id;
    }
    virtual uint64_t getID() const
	{
        return m_ID;
	}

    //add for threadPool
    virtual void recvWorkItem( ThreadPoolWorkItem* ) {}

private:
    uint64_t m_ID;
};

#endif

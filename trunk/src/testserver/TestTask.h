#ifndef _TESTTASK_H
#define _TESTTASK_H

#include "common/comm/BaseTask.h"

#include <stdint.h>
#include <string>
using std::string;

const uint32_t DO_LOG = 1;
const uint32_t WAIT_WORKITEM = 2;
const uint32_t FINISH = 3;

class TestTask : public BaseTask
{
public:
	TestTask();
	~TestTask();

    int goNext();
	void recvWorkItem( ThreadPoolWorkItem* );

    void setNum(int num)
    	{
    	    m_testnum = num;
    	}
	void setData(const string &data)
	{
	    m_data = data;
	}
	
private:
	uint32_t m_state;
	int m_testnum;
	string m_data;
};

#endif //_TESTTASK_H
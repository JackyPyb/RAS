#ifndef _TESTWORKITEM_H
#define _TESTWORKITEM_H

#include "common/sys/ThreadPoolWorkItem.h"

#include <string>
using std::string;

class TestWorkItem : public ThreadPoolWorkItem
{
public:
	TestWorkItem();
	~TestWorkItem();

	int process();

	void setData(const string &data)
	{
	    m_logData = data;
	}

private:
	string m_logData;
};

#endif //_TESTWORKITEM_H
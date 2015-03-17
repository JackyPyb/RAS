#ifndef _RC_CONTAINRESOURCETASK_H_
#define _RC_CONTAINRESOURCETASK_H_

#include "Task.h"
#include <Resource.h>

#include <map>
#include <string>

using std::multimap;
using std::string;

namespace rc
{

class ContainResourceTask : public Task
{
public:
    ContainResourceTask(){}
    virtual ~ContainResourceTask()
    {
        m_resourceMap.clear();
    }

    void setResourceMap(const multimap<string, Resource> &resMap)
    {
        m_resourceMap = resMap;
    }

    multimap<string, Resource> getResourceMap() const
    {
        return m_resourceMap;
    }

private:
    multimap<string, Resource> m_resourceMap;
};    

}

#endif

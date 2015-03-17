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
    virtual ~ContainResourceTask(){}

    void setResourceMap(const multimap<string, Resource> &);
    multimap<string, Resource> getResourceMap() const;

private:
    multimap<string, Resource> m_resourceMap;
};    

}

#endif

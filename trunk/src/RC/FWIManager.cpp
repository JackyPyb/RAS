#include <algorithm>

#include "FWInstance.h"
#include "FWIManager.h"

namespace rc
{

FWIManager::FWIManager():
    m_FWIID(0),
    m_moduleID(0),
    m_map()
{

}

FWIManager::~FWIManager()
{
    recycleAll();    
}

FWInstance* FWIManager::createFWInstance()
{
    FWInstance *fwinstance = new FWInstance();
    if(fwinstance != NULL)
    {
        add(fwinstance);
    }

    return fwinstance;
}

uint32_t FWIManager::add(FWInstance *fwinstance)
{
    uint32_t id = generateInstanceID();
    m_map[id] = fwinstance;
    fwinstance->setFWInstanceID(id);
    return id;
}

FWInstance* FWIManager::get(uint32_t id) const
{
    FWInstance* fwinstance = NULL;
    FWIMap::const_iterator iter = m_map.find(id);
    if(iter != m_map.end())
    {
        fwinstance = iter->second;
    }

    return fwinstance;
}

bool FWIManager::find(uint32_t id) const
{
    FWIMap::const_iterator iter = m_map.find(id);
    if(iter != m_map.end())
    {
        return true;
    }

    return false;
}

bool FWIManager::remove(uint32_t id)
{
    FWIMap::size_type eraseCount = m_map.erase(id);
    return (eraseCount != 0);
}

void FWIManager::recycle(uint32_t id)
{
    FWInstance *fwinstance = NULL;
    if(find(id))
    {
        fwinstance = get(id);
        remove(id);
        delete fwinstance;
        fwinstance = NULL;
    }
}

void FWIManager::recycleAll()
{
    if(!m_map.empty())
    {
        FWIMap::iterator iter = m_map.begin();
        for(; iter != m_map.end(); iter++)
        {
            delete iter->second;
            iter->second = NULL;
        }

        m_map.clear();
    }
}

uint32_t FWIManager::generateInstanceID()
{
    ++m_FWIID;
    return m_FWIID;
}

uint32_t FWIManager::generateModuleID()
{
    ++m_moduleID;
    return m_moduleID;
}

}

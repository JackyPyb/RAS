#include "StrategyManager.h"
#include "GetNCFromAll.h"
#include "GetNCFromOne.h"

#include "common/log/log.h"

namespace rc
{

StrategyManager::StrategyManager():
    m_sID(0)
{

}

StrategyManager::~StrategyManager()
{
    createStrategy<GetNCFromAll>();
    createStrategy<GetNCFromOne>();
}

void StrategyManager::init()
{
    INFO_LOG("StrategyManager::init createStrategy");
    createStrategy<GetNCFromAll>();
    createStrategy<GetNCFromOne>();
}

uint32_t StrategyManager::addStrategy(Strategy *pStrategy)
{
    uint32_t id = generateStrategyID();
    INFO_LOG("StrategyManager::addStrategy id is %d", id);
    pStrategy->setStrategyID(id);
    m_strategyMap[id] = pStrategy;
    return id;
}

bool StrategyManager::deleteStrategy(const uint32_t sid)
{
    Strategy *pStrategy = getStrategy(sid);
    StrategyMap::size_type eraseCount;

    eraseCount = m_strategyMap.erase(sid);
    if(pStrategy != NULL)
    {
        delete pStrategy;
        pStrategy = NULL;
    }

    return (eraseCount != 0);
}

Strategy* StrategyManager::getStrategy(const uint32_t sid)
{
    StrategyMap::const_iterator iter;
    Strategy *pStrategy = NULL;

    iter = m_strategyMap.find(sid);
    if(iter != m_strategyMap.end())
    {
        pStrategy = iter->second;
    }
    else
    {
        INFO_LOG("Can not find strategy");
    }

    return pStrategy;
}

uint32_t StrategyManager::generateStrategyID()
{
    return ++m_sID;
}

}

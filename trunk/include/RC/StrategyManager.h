#ifndef _RC_STRATEGYMANAGER_H_
#define _RC_STRATEGYMANAGER_H_

#include "common/sys/Singleton.h"
#include "Strategy.h"

#include <map>
#include <stdint.h>

using std::map;

namespace rc
{

class StrategyManager : public Singleton<StrategyManager>
{
protected:
    StrategyManager();
    ~StrategyManager();

public:
    friend class Singleton<StrategyManager>;

    template<typename T>
        T* createStrategy(void);

    void init();

    uint32_t addStrategy(Strategy*);
    bool deleteStrategy(const uint32_t);
    Strategy* getStrategy(const uint32_t);
    uint32_t generateStrategyID();

private:
    typedef map<uint32_t, Strategy*> StrategyMap;

    StrategyMap m_strategyMap;
    uint32_t m_sID;

};

template<typename T>
T* StrategyManager::createStrategy(void)
{
    T* pStrategy = new T();
    addStrategy(pStrategy);
    return pStrategy;
}

}

#endif

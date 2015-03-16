#ifndef _RC_FWIMANAGER_H_
#define _RC_FWIMANAGER_H_

#include <map>

#include "common/sys/Singleton.h"

using std::map;

namespace rc
{
class FWInstance;

class FWIManager : public Singleton<FWIManager>
{
public:
    friend class Singleton<FWIManager>;

    uint32_t add(FWInstance*);
    FWInstance* get(uint32_t) const;
    bool find(uint32_t) const;
    bool remove(uint32_t);
    void recycle(uint32_t);
    void recycleAll();

    FWInstance* createFWInstance();

private:
    FWIManager();
    ~FWIManager();

    typedef map<uint32_t, FWInstance*> FWIMap;

    uint32_t generateInstanceID();
    uint32_t generateModuleID();

    FWIMap m_map;
    uint32_t m_FWIID;
    uint32_t m_moduleID;
};
}
#endif

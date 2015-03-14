#ifndef _RC_RUNCONTROL_H_
#define _RC_RUNCONTROL_H_

#include "common/sys/Singleton.h"

namespace rc
{

class RunControl : public Singleton<RunControl>
{
public:
    friend class Singleton<RunControl>;

    void run(void);
    void runRC(void);
    void NCListen(void);
    void SASListen(void);
    void epollRun(void);
private:
    RunControl();
    ~RunControl();
};

}

#endif

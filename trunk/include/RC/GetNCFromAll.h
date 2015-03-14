#ifndef _RC_GETNCFROMALL_H_
#define _RC_GETNCFROMALL_H_

#include "Strategy.h"

namespace rc
{

class GetNCFromAll : public Strategy
{
public:
    int dispatch(Task*);

};

}

#endif

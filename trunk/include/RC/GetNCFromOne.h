#ifndef _RC_GETNCFROMONE_H_
#define _RC_GETNCFROMONE_H_

#include "Strategy.h"

namespace rc
{

class GetNCFromOne : public Strategy
{
public:
    int dispatch(Task*);

};

}

#endif

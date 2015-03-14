#include "TestWorkItem.h"

#include "common/comm/Error.h"
#include "common/log/log.h"

TestWorkItem::TestWorkItem():
     m_logData("")
{
}

TestWorkItem::~TestWorkItem()
{
}

int
TestWorkItem::process()
{
    INFO_LOG("data from client: %s", m_logData.c_str());
    return SUCCESSFUL;
}


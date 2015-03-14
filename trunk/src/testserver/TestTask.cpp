#include "TestTask.h"
#include "TestWorkItem.h"

#include "common/sys/ThreadPoolDispatcher.h"
#include "common/comm/TaskManager.h"
#include "common/log/log.h"

const int everylognum = 1000;

TestTask::TestTask():
    m_state(DO_LOG),
    m_testnum(0),
    m_data("")
{
}

TestTask::~TestTask()
{
}

int
TestTask::goNext()
{
    switch ( m_state )
    {
        case DO_LOG:
        {
            if ( 0 == m_testnum%everylognum )
            {
                TestWorkItem *pWI = new TestWorkItem();
                pWI->setData(m_data);
                pWI->setTaskID(getID());
                g_pDispatcher->postRequest(pWI);
            }
            m_state = WAIT_WORKITEM;
            goNext();
            break;
        }
        case WAIT_WORKITEM:
            if ( 0 != m_testnum%everylognum )
            {
                m_state = FINISH;
                goNext();
            }
            break;
        case FINISH:
            TaskManager::getInstance()->recycle(getID());
            break;
        default:
            ERROR_LOG("TestTask no this state");
            return FAILED;
            break;
    }
    return SUCCESSFUL;
}

void
TestTask::recvWorkItem(ThreadPoolWorkItem *pWorkItem)
{
    if ( pWorkItem != NULL)
    {
        delete pWorkItem;
        pWorkItem = NULL;
    }

    m_state = FINISH;
    goNext();
}

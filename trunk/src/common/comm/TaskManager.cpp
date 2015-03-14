/*
 * =====================================================================================
 *
 *       Filename:  TaskManager.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/28/2012 06:25:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  sudong (sd), sudongndsl@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#include <cassert>
#include <algorithm>

#include "common/comm/BaseTask.h"
#include "common/comm/TaskManager.h"

using std::for_each;

namespace
{

class TaskDeleter
{
public:
    void operator()( BaseTask* task )const
    {
        if ( task != NULL )
        {
            delete task;
            task = NULL;
        }
    }
};

class GeneralIDGenerator
{
public:
    GeneralIDGenerator(): m_ID( 0 ) {}
    uint64_t operator()( void )
    {
        ++m_ID;
        return m_ID;
    }
private:
    uint64_t m_ID;
};

}

uint64_t TaskManager::add(
    BaseTask* task )
{
    uint64_t id;

    id = generateID();
    m_map[id] = task;
    task->setID( id );
    return id;
}

BaseTask* TaskManager::get(
    uint64_t id )const
{
    BaseTask* task = NULL;

    TaskMap::const_iterator it = m_map.find( id );
    if ( it != m_map.end() )
    {
        task = it->second;
    }
    return task;
}

bool TaskManager::find(
    uint64_t id )const
{
    TaskMap::const_iterator it = m_map.find( id );
    if ( it != m_map.end() )
    {
        return true;
    }
    return false;
}

bool TaskManager::remove(
    uint64_t id )
{
    TaskMap::size_type eraseCount = m_map.erase( id );
    return ( eraseCount != 0 );
}

void TaskManager::recycle(
    uint64_t id )
{
    if ( find( id ) )
    {
        BaseTask* task = get( id );
        if ( !remove( id ) )
        {
            return;
        }
        TaskList::iterator tlit = 
            m_recycleList.insert( m_recycleList.end(), task );
        if ( tlit != m_recycleList.end())
        {
        }
    }
}

void TaskManager::recycleAll()
{
    if ( !m_recycleList.empty() )
    {
        for_each( m_recycleList.begin(), m_recycleList.end(), TaskDeleter() );
        m_recycleList.clear();
    }
    return;
}

TaskManager::IDGenerator TaskManager::setIDGenerator(
    const IDGenerator& newGenerator )
{
    IDGenerator oldGenerator( m_IDGenerator );
    m_IDGenerator = newGenerator;
    return oldGenerator;
}

TaskManager::IDGenerator TaskManager::getIDGenerator()const
{
    return m_IDGenerator;
}

TaskManager::TaskManager():
    m_map(),
    m_recycleList(),
    m_IDGenerator( GeneralIDGenerator() )
{
}

TaskManager::~TaskManager()
{

}

void
TaskManager::clearAllTask()
{
    if ( !m_map.empty() )
    {
        for ( TaskMap::iterator it = m_map.begin();
                it != m_map.end();)
        {
            TaskMap::iterator tempit = it;
            ++it;
            recycle( tempit->first );
        }
    }
}

uint64_t TaskManager::generateID()
{
    return m_IDGenerator();
}

/*
 * =====================================================================================
 *
 *       Filename:  TaskManager.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/28/2012 06:26:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  sudong (sd), sudongndsl@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef BASE_TASK_MANAGER_H
#define BASE_TASK_MANAGER_H

#include <stdint.h>
#include <map>
#include <list>
#include <tr1/functional>

#include "common/sys/Singleton.h"

class BaseTask;

class TaskManager: public Singleton<TaskManager>
{
    friend class Singleton<TaskManager>;

public:
    typedef std::tr1::function<uint64_t ( void )> IDGenerator;
    //create a task.
    template<typename T>
    T* create( void );
    //add a task to taskmanager
    uint64_t add( BaseTask* const );
    //get a task base on task id.
    BaseTask* get( uint64_t ) const;
    //is there have a task in Taskmanager?
    bool find( uint64_t ) const;
    //taskmanager will not manager the task.
    bool remove( uint64_t );
    //add a task to recycle list.
    void recycle( uint64_t );
    //destructor the  task in recycle list.
    void recycleAll( void );

	void clearAllTask();

    //set the ID generator.
    //IDGenerator must generate a unique task id.
    IDGenerator setIDGenerator( const IDGenerator& );
    //get the current ID generator
    IDGenerator getIDGenerator( void )const;

private:
    typedef std::map<uint64_t, BaseTask*> TaskMap;
    typedef std::list<BaseTask*> TaskList;

    TaskManager();
    ~TaskManager();
    uint64_t generateID( void );

private:
    TaskMap m_map;
    TaskList m_recycleList;
    IDGenerator m_IDGenerator;
};

template <typename T>
T* TaskManager::create()
{
    T* t;
    uint64_t id;

    t = new T();
    if ( t != NULL )
    {
        id = add( t );
    }

    return t;
}

#endif

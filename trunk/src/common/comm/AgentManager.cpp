#include <cassert>
#include <algorithm>

#include "common/comm/Agent.h"
#include "common/comm/AgentManager.h"
#include "common/comm/Epoll.h"
#include "common/comm/SocketAddress.h"
#include "common/comm/TCPSocket.h"

#include <cstdio>
using std::for_each;

namespace
{

struct AgentDeleter
{
    void operator()( Agent* agent )const
    {
        printf( "%p\n",agent);
        if ( agent != NULL)
        {
            delete agent;
            agent = NULL;
        }
    }
};

}

AgentManager::AgentManager():
    m_agentID( 0 ),
    m_map(),
    m_recycleList()
{
}

AgentManager::~AgentManager()
{

}

uint32_t AgentManager::add( Agent* agent )
{
    uint32_t id = generateID();
    m_map[id] = agent;
    agent->setID( id );
    return id;
}

Agent* AgentManager::get( uint32_t id )const
{
    Agent* agent = NULL;
    AgentMap::const_iterator it = m_map.find( id );
    if ( it != m_map.end() )
    {
        agent = it->second;
    }
    return agent;
}

bool AgentManager::find( uint32_t id )const
{
    AgentMap::const_iterator it = m_map.find( id );
    if ( it != m_map.end() )
    {
        return true;
    }
    return false;
}

bool AgentManager::remove( uint32_t id )
{
    AgentMap::size_type eraseCount = m_map.erase( id );
    return ( eraseCount != 0 );
}

void AgentManager::recycle( uint32_t id )
{
    Agent* agent = NULL;

    if ( find( id ) )
    {
        agent = get( id );
        if ( !remove( id ) )
        {
        }
        AgentList::iterator ait = 
            m_recycleList.insert( m_recycleList.end(), agent );
        if ( ait == m_recycleList.end() )
        {
        }

        agent->recycler();
    }
    return;
}

void AgentManager::recycleAll()
{
    if ( !m_recycleList.empty() )
    {
        for_each( 
            m_recycleList.begin(), m_recycleList.end(), AgentDeleter() );
        m_recycleList.clear();
    }
    return;
}

void
AgentManager::clearAllAgent()
{
    if ( !m_map.empty() )
    {
        for (AgentMap::iterator it = m_map.begin();
            it != m_map.end();)
        {
            AgentMap::iterator tempit = it;
            ++it;
            recycle( tempit->first );
        }
    }
}

uint32_t AgentManager::generateID()
{
    ++m_agentID;
    return m_agentID;
}

void AgentManager::recyclerAgent( Agent* agent )
{
    if ( agent != NULL )
    {
        agent->recycler();
        recycle( agent->getID() );
    }
    return;
}

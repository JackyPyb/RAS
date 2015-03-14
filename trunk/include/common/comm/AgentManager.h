#ifndef _AGENTMANAGER_H
#define _AGENTMANAGER_H

#include <map>
#include <list>
#include <stdint.h>

#include "common/sys/Singleton.h"

class Agent;
class Epoll;
class SocketAddress;
class TCPSocket;

class AgentManager: public Singleton<AgentManager>
{
    friend class Singleton<AgentManager>;
public:
    //add a agent to agentManager.
    uint32_t add( Agent* const );
    //get a agent base on id.
    Agent* get( uint32_t ) const;
    //find is there have a agent's ID is id.
    bool find( uint32_t ) const;
    //agentmanager will not manager the agent.
    bool remove( uint32_t );
    //add a agent to recycle list.
    void recycle( uint32_t );
    //destructor agent in recycle list.
    void recycleAll( void );

	void clearAllAgent();

    template<typename T>
    T* createAgent();
    template<typename T>
    T* createAgent( const SocketAddress& );
    template<typename T>
    T* createAgent( const TCPSocket& , const SocketAddress& );
    void recyclerAgent( Agent* );

private:
    AgentManager();
    ~AgentManager();

private:
    typedef std::map<uint32_t, Agent*> AgentMap;
    typedef std::list<Agent*> AgentList;

    uint32_t generateID( void );

    uint32_t m_agentID;
    AgentMap m_map;
    AgentList m_recycleList;
};

template<typename T>
T* AgentManager::createAgent()
{
    T* agent;
    agent = new T();
    if ( agent != NULL )
    {
        add( agent );
    }

    return agent;
}

template<typename T>
T* AgentManager::createAgent( const SocketAddress& addr )
{
    T* agent;
    agent = new T( addr );
    if ( agent != NULL )
    {
        add( agent );
    }

    return agent;
}

template<typename T>
T* AgentManager::createAgent(
    const TCPSocket& sock,
    const SocketAddress& addr
    )
{
    T* agent;
    agent = new T( sock, addr );
    if ( agent != NULL )
    {
        add( agent );
    }

    return agent;
}

#endif


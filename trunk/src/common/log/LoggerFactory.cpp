/*
 * =====================================================================================
 *
 *       Filename:  LoggerFactory.cpp
 *
 *    Description:  日志工厂类
 *
 *        Version:  1.0
 *        Created:  04/24/2011 05:34:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */


#include <cstdio>
#include <cstdlib>
#include <utility>

#include "common/log/Logger.h"
#include "common/log/ConsoleAppender.h"
#include "common/log/SyslogAppender.h"

// logger map
std::map<std::string, Logger*> LoggerFactory::m_loggerMap;

LoggerFactory::LoggerFactory()
{
    // init ROOT Logger
    Logger* rootLogger = new Logger();
    rootLogger->setName( "ROOT" );
#ifdef DEBUG
    rootLogger->setLevel( LOG_L_DEBUG );
#else
    rootLogger->setLevel( LOG_L_INFO );
#endif

    Appender* p_Appender;
    p_Appender = new SyslogAppender();
    if ( p_Appender->init() != 0 )
    {
        fprintf(
            stderr,
            "Error occurred while initializing the ROOT logger. Program exited.\n" );
        exit( 1 );
    }
    rootLogger->addAppender( p_Appender );

    std::pair< std::map<std::string, Logger*>::iterator, bool > logpair = 
        m_loggerMap.insert(
            std::pair<std::string, Logger*>(
                "ROOT", rootLogger ) );
    if ( !logpair.second )
    {
    }
}

LoggerFactory::~LoggerFactory()
{

}

Logger* LoggerFactory::getRootLogger() const
{
    return m_loggerMap["ROOT"];
}

void
LoggerFactory::clear()
{
    if ( !m_loggerMap.empty() )
    {
        for ( std::map<std::string, Logger*>::iterator it = m_loggerMap.begin();
                it != m_loggerMap.end();
                 ++it)
        {        
            delete it->second;
            it->second = NULL;
        }
    }
}

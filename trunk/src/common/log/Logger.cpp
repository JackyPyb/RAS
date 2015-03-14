/*
 * =====================================================================================
 *
 *       Filename:  Logger.cpp
 *
 *    Description:  日志类实现
 *
 *        Version:  1.0
 *        Created:  04/24/2011 02:08:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#include <cstdarg>

#include "common/log/Appender.h"
#include "common/log/Logger.h"

Logger::Logger()
    : m_level( LOG_L_DEBUG )
{

}

Logger::~Logger()
{
    try
    {
        // 析构每个Appender
        for ( std::list<Appender*>::iterator it = m_appenderList.begin();
                it != m_appenderList.end();)
        {
            std::list<Appender*>::iterator tmpit = it;
            ++it;
            delete (*tmpit);
            *tmpit = NULL;
        }
        m_appenderList.clear();
    }
    catch ( ... )
    {
    }
}

void Logger::setLevel( LogLevel level )
{
    m_level = level;
}

void Logger::setName( const std::string& name )
{
    m_name = name;
}

void Logger::addAppender( Appender* apdr )
{
    m_appenderList.push_back( apdr );
}

void Logger::doLog(
    LogLevel level,
    const char* fmt,
    va_list ap,
    const char* filename,
    int linenum,
    const char* function )
{
    // 把日志写入到每个Appender
    for ( std::list<Appender*>::iterator it = m_appenderList.begin();
            it != m_appenderList.end();
            ++it )
    {
        if ( level == LOG_L_DEBUG )  // 如果是调试信息
        {
            if ( NULL == filename || NULL == function )
            {
                ( *it )->log( m_name, level, fmt, ap, "NULL", linenum, "NULL" );
            }
            else
            {
                ( *it )->log( m_name, level, fmt, ap, filename, linenum, function );
            }
        }
        else
        {
            ( *it )->log( m_name, level, fmt, ap );
        }
    }
}

void Logger::perf( const char* fmt, ... )
{
    // 如果设定的日志级别比FATAL低，则记录日志
    if ( LOG_L_PERF >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_PERF, fmt, ap );
        va_end( ap );
    }
}

void Logger::fatal( const char* fmt, ... )
{
    // 如果设定的日志级别比FATAL低，则记录日志
    if ( LOG_L_FATAL >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_FATAL, fmt, ap );
        va_end( ap );
    }
}

void Logger::error( const char* fmt, ... )
{
    if ( LOG_L_ERROR >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_ERROR, fmt, ap );
        va_end( ap );
    }
}

void Logger::warn( const char* fmt, ... )
{
    if ( LOG_L_WARN >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_WARN, fmt, ap );
        va_end( ap );
    }
}

void Logger::info( const char* fmt, ... )
{
    if ( LOG_L_INFO >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_INFO, fmt, ap );
        va_end( ap );
    }
}

void Logger::debug(
    const char* filename,
    int linenum,
    const char* function,
    const char* fmt, ... )
{
    if ( LOG_L_DEBUG >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_DEBUG, fmt, ap, filename, linenum, function );
        va_end( ap );
    }
}

void Logger::trace( const char* fmt, ... )
{
    if ( LOG_L_TRACE >= m_level )
    {
        va_list ap;

        va_start( ap, fmt );
        doLog( LOG_L_TRACE, fmt, ap );
        va_end( ap );
    }
}


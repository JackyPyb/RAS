/*
 * =====================================================================================
 *
 *       Filename:  Appender.cpp
 *
 *    Description:  日志输出器
 *
 *        Version:  1.0
 *        Created:  04/24/2011 02:55:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */


#include <cstdio>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "common/log/Appender.h"

std::string Appender::getTimeNow() const
{
    time_t t;
    time_t rt = time( &t ); // get now time
    if ( rt < 0 )
    {
    }

    struct tm pt;  // parsed time
    struct tm *rttm = localtime_r( &t, &pt );
    if ( rttm == NULL)
    {
    }

    char time_str[30];  // time has format 2011-04-24 15:04:22
    int rtpr = snprintf( 
                      time_str, 
                      30, 
                      "%04d-%02d-%02d %02d:%02d:%02d",
                      pt.tm_year + 1900,
                      pt.tm_mon + 1,
                      pt.tm_mday,
                      pt.tm_hour,
                      pt.tm_min,
                      pt.tm_sec );
    if ( rtpr < 0 )
    {
    }
    
    return std::string( time_str );
}

std::string Appender::getLogLevelString( LogLevel level ) const
{
    return std::string( "[" ) + std::string( LOG_LEVEL_STR[level] ) + "]";
}

std::string Appender::getPID() const
{
    char pid[20];

    sprintf( pid, "0x%x", getpid() );

    return std::string( pid );
}

std::string Appender::getTID() const
{
    char tid[40];

    sprintf( tid, "0x%lx", ( unsigned long )pthread_self() );

    return std::string( tid );
}

std::string Appender::getPIDAndTID() const
{
    return std::string( "[" ) + getPID()
           + ":" + getTID() + "]";
}

void
Appender::log(
    std::string,
    LogLevel,
    const char*,
    va_list,
    const char*,
    int,
    const char* )
{}


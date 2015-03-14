/*
 * =====================================================================================
 *
 *       Filename:  SyslogAppender.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/26/2013 05:49:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  sudong (sd), sudongndsl@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include <string.h>
#include <syslog.h>
#include <stdio.h>
#include <iostream>
#include <inttypes.h>

#include "common/log/SyslogAppender.h"

const uint32_t LOG_BUFFER_SIZE = 6000;

SyslogAppender::SyslogAppender():
    Appender()
{
}

SyslogAppender::~SyslogAppender()
{
    closelog();
}

int SyslogAppender::init()
{
    openlog( NULL, LOG_PID, LOG_USER );
    return SUCCESSFUL;
}

void SyslogAppender::log(
    std::string loggerName,
    LogLevel level,
    const char* fmt,
    va_list ap,
    const char* filename,
    int linenum,
    const char* function )
{
    char buffer[LOG_BUFFER_SIZE] = "";

    // time, levle and logger name
    int rt = snprintf( buffer, LOG_BUFFER_SIZE, "%s - ",
                  loggerName.c_str() );
    if ( rt < 0)
    {
    }

    // log data
    size_t len = strlen( buffer );
    rt = vsnprintf( buffer + len, LOG_BUFFER_SIZE - len, fmt, ap );
    if ( rt < 0)
    {
    }

    len = strlen( buffer );
    // 添加换行
    buffer[len++] = '\n';
    buffer[len] = '\0';
    if ( level == LOG_L_DEBUG 
        && filename != NULL 
        && linenum != 0 
        && function != NULL )
    {
        // 去掉换行
        while ( buffer[len - 2] == '\n' && len > 2 )
        {
            --len;
        }
        buffer[len] = '\0';

        rt = snprintf( buffer + len, LOG_BUFFER_SIZE - len,
                  " (IN FILE: %s, LINE: %d, FUNCTION: %s)\n",
                  filename, linenum, function );
        if ( rt < 0)
        {
        }
    }

    len = strlen( buffer );
    // 去掉多余的空行
    while ( buffer[len - 2] == '\n' && len > 2 )
    {
        --len;
    }
    buffer[len] = '\0';

    switch ( level )
    {
    case LOG_L_TRACE:
    case LOG_L_DEBUG:
        ::syslog( LOG_USER | LOG_DEBUG, buffer );
        break;
    case LOG_L_INFO:
        ::syslog( LOG_USER | LOG_INFO, buffer );
        break;
    case LOG_L_WARN:
        ::syslog( LOG_USER | LOG_WARNING, buffer );
        break;
    case LOG_L_ERROR:
        ::syslog( LOG_USER | LOG_ERR, buffer );
        break;
    case LOG_L_FATAL:
        ::syslog( LOG_USER | LOG_EMERG, buffer );
        break;
    case LOG_L_PERF:
        ::syslog( LOG_LOCAL0 | LOG_INFO, buffer );
        break;
    default:
        std::cerr << "UNKNOWN LOG LEVEL" << std::endl;
        break;
    }
    return;
}

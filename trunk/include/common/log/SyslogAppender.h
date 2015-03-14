/*
 * =====================================================================================
 *
 *       Filename:  SyslogAppender.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  02/26/2013 05:50:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  sudong (sd), sudongndsl@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef COMMON_LOG_SYSLOG_APPENDER_HEADER
#define COMMON_LOG_SYSLOG_APPENDER_HEADER

#include "common/log/Appender.h"

class SyslogAppender: public Appender
{
public:
    SyslogAppender();
    ~SyslogAppender();
	
public:
    int init( void );
    void log(
        std::string loggerName,
        LogLevel level,
        const char* fmt,
        va_list ap,
        const char* filename = NULL,
        int linenum = 0,
        const char* function = NULL );
};

#endif

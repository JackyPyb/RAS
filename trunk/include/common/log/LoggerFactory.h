/*
 * =====================================================================================
 *
 *       Filename:  LoggerFactory.h
 *
 *    Description:  日志工厂类
 *
 *        Version:  1.0
 *        Created:  04/24/2011 04:34:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:
 *
 * =====================================================================================
 */

#ifndef COMMON_LOG_LOGGERFACTORY_H_
#define COMMON_LOG_LOGGERFACTORY_H_

#include <map>
#include <string>

#include "common/sys/Singleton.h"
#include "common/log/LogLevel.h"

class Logger;

/**
 * @brief 日志工厂类
 */
class LoggerFactory : public Singleton<LoggerFactory>
{
    friend class Singleton<LoggerFactory>;
public:

    /**
     * @brief 获取指定Logger
     *
     * @param loggerName Logger的名称
     *
     * @return 指向Logger的指针
     *      - NULL 指定的Logger不存在
     */
    Logger* getLogger( const std::string& loggerName );

    /**
     * @brief 获取ROOT Logger（ROOT Logger总是存在）
     *
     * @return 指向ROOT Logger的指针
     */
    Logger* getRootLogger() const;
    void clear();

private:
    LoggerFactory();  // can't new
    LoggerFactory& operator=( const LoggerFactory& buffer );
	~LoggerFactory();

private:
    static std::map<std::string, Logger*> m_loggerMap;   // map of logger
};

#endif // COMMON_LOG_LOGGERFACTORY_H_


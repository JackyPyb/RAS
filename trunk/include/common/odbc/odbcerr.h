/*
 * =============================================================================
 *
 *       Filename:  odbcerr.h
 *
 *    Description:  提取ODBC错误信息
 *
 *        Version:  1.0
 *        Created:  2011-05-07 13:23:12
 *  Last Modified:  2011-05-07 13:25:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */


#ifndef COMMON_ODBCERR_H_
#define COMMON_ODBCERR_H_

#include <string>
#include <sql.h>

namespace odbc
{
/**
 * @brief 提取ODBC错误信息
 *
 * @param fn 出错的函数名称
 * @param handleType ODBC句柄类型
 * @param handle ODBC句柄
 *
 * @return 错误信息
 */
std::string getODBCError( 
    const char* fn, SQLSMALLINT handleType, SQLHANDLE handle );
}  // namespace odbc

#endif  // COMMON_ODBCERR_H_

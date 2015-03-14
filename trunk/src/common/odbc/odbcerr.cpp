/*
 * =============================================================================
 *
 *       Filename:  odbcerr.cpp
 *
 *    Description:  提取ODBC错误信息
 *
 *        Version:  1.0
 *        Created:  2011-05-07 13:25:54
 *  Last Modified:  2011-05-07 13:25:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */


#include <cstdio>

#include "common/odbc/odbcerr.h"

std::string odbc::getODBCError(
    const char* fn,
    SQLSMALLINT handleType,
    SQLHANDLE handle )
{
    SQLINTEGER i = 0;
    SQLINTEGER native;
    SQLCHAR state[7];
    SQLCHAR text[1024];
    SQLSMALLINT len;
    SQLRETURN ret;

    char buffer[1024];

    std::string error =
        "The driver reported the following diagnostics whilst running ";
    error = error + fn + ": \n";

    // 从ODBC提取错误信息
    do
    {
        ret = SQLGetDiagRec( handleType, handle, ++i, state, &native, text,
                             sizeof( text ), &len );
        if ( SQL_SUCCEEDED( ret ) )
        {
            int rt = snprintf(
                          buffer,
                          1024,
                          "\t%s:%ld:%ld:%s\n",
                          state,
                          ( long )i,
                          ( long )native,
                          text );
            if ( rt < 0 )
            {
            }
            error += buffer;
        }
    }
    while ( SQL_SUCCESS == ret );

    return error;
}


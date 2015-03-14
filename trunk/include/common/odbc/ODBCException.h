/*
 * =====================================================================================
 *
 *       Filename:  ODBCException.h
 *
 *    Description:  ODBC异常
 *
 *        Version:  1.0
 *        Created:  2011-05-27 16:51:34
 *  Last Modified:  2011-05-27 16:51:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef COMMON_ODBC_ODBCEXCEPTION_
#define COMMON_ODBC_ODBCEXCEPTION_

#include "common/sys/Exception.h"

class ODBCException : public Exception
{
public:
    ODBCException();
    ODBCException( const std::string& msg );
    ODBCException( const std::string& filename,
                   int linenum );
    ODBCException(
        const std::string& msg,
        const std::string& filename,
        int linenum );

    virtual std::string name() const;

private:
    static const std::string m_name;
};

#endif  // COMMON_ODBC_ODBCEXCEPTION_


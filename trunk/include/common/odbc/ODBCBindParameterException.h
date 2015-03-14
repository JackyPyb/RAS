/*
 * =====================================================================================
 *
 *       Filename:  ODBCBindParameterException.h
 *
 *    Description:  参数绑定异常
 *
 *        Version:  1.0
 *        Created:  2011-05-27 17:05:59
 *  Last Modified:  2011-05-27 17:05:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef COMMON_ODBC_ODBCBINDPARAMETEREXCEPTION_H_
#define COMMON_ODBC_ODBCBINDPARAMETEREXCEPTION_H_

#include "common/odbc/ODBCException.h"

class ODBCBindParameterException : public ODBCException
{
public:
    ODBCBindParameterException( int idx = 0 );
    ODBCBindParameterException(
        const std::string& msg,
        int idx = 0 );
    ODBCBindParameterException(
        const std::string& filename,
        int linenum,
        int idx = 0 );
    ODBCBindParameterException(
        const std::string& msg,
        const std::string& filename,
        int linenum,
        int idx = 0 );

    virtual std::string name() const;

    virtual int getParameterIndex() const;

private:
    static const std::string m_name;
    int m_paramIndex;  // 第几个参数
};

#endif  // COMMON_ODBC_ODBCBINDPARAMETEREXCEPTION_H_


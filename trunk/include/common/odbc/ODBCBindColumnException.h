/*
 * =====================================================================================
 *
 *       Filename:  ODBCBindColumnException.h
 *
 *    Description:  列绑定异常
 *
 *        Version:  1.0
 *        Created:  2011-05-27 17:28:45
 *  Last Modified:  2011-05-27 17:28:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef COMMON_ODBC_ODBCBINDCOLUMNEXCEPTION_H_
#define COMMON_ODBC_ODBCBINDCOLUMNEXCEPTION_H_

#include "common/odbc/ODBCException.h"

class ODBCBindColumnException : public ODBCException
{
public:
    ODBCBindColumnException( int idx = 0 );
    ODBCBindColumnException(
        const std::string& msg,
        int idx = 0 );
    ODBCBindColumnException(
        const std::string& filename,
        int linenum,
        int idx = 0 );
    ODBCBindColumnException(
        const std::string& msg,
        const std::string& filename,
        int linenum,
        int idx = 0 );

    virtual std::string name() const;

    virtual int getColumnIndex() const;

private:
    static const std::string m_name;
    int m_columnIndex;  // 第几列
};

#endif  // COMMON_ODBC_ODBCBINDCOLUMNEXCEPTION_H_



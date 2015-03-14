/*
 * =============================================================================
 *
 *       Filename:  ODBCBindParameterException.cpp
 *
 *    Description:  参数绑定异常
 *
 *        Version:  1.0
 *        Created:  2011-05-27 17:15:23
 *  Last Modified:  2011-05-27 17:15:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include "common/odbc/ODBCBindParameterException.h"

ODBCBindParameterException::ODBCBindParameterException( int idx ) :
    m_paramIndex( idx )
{

}

ODBCBindParameterException::ODBCBindParameterException(
    const std::string& filename,
    int linenum,
    int idx ) :
    ODBCException( filename, linenum ),
    m_paramIndex( idx )
{

}

ODBCBindParameterException::ODBCBindParameterException(
    const std::string& msg,
    const std::string& filename,
    int linenum,
    int idx ) :
    ODBCException( msg, filename, linenum ),
    m_paramIndex( idx )
{

}

const std::string
ODBCBindParameterException::m_name = "ODBCBindParameterException";

std::string
ODBCBindParameterException::name() const
{
    return m_name;
}

int
ODBCBindParameterException::getParameterIndex() const
{
    return m_paramIndex;
}

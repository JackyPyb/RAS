/*
 * =============================================================================
 *
 *       Filename:  ODBCBindColumnException.cpp
 *
 *    Description:  列绑定异常
 *
 *        Version:  1.0
 *        Created:  2011-05-27 17:33:30
 *  Last Modified:  2011-05-27 17:33:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include "common/odbc/ODBCBindColumnException.h"

ODBCBindColumnException::ODBCBindColumnException(
    int idx ) :
    m_columnIndex( idx )
{

}

ODBCBindColumnException::ODBCBindColumnException(
    const std::string& msg,
    int idx ) :
    ODBCException( msg ),
    m_columnIndex( idx )
{

}

ODBCBindColumnException::ODBCBindColumnException(
    const std::string& filename,
    int linenum,
    int idx ) :
    ODBCException( filename, linenum ),
    m_columnIndex( idx )
{

}

ODBCBindColumnException::ODBCBindColumnException(
    const std::string& msg,
    const std::string& filename,
    int linenum,
    int idx ) :
    ODBCException( msg, filename, linenum ),
    m_columnIndex( idx )
{

}

const std::string
ODBCBindColumnException::m_name = "ODBCBindColumnException";

std::string
ODBCBindColumnException::name() const
{
    return m_name;
}

int
ODBCBindColumnException::getColumnIndex() const
{
    return m_columnIndex;
}


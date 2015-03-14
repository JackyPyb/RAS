/*
 * =============================================================================
 *
 *       Filename:  ODBCException.cpp
 *
 *    Description:  ODBC异常
 *
 *        Version:  1.0
 *        Created:  2011-05-27 16:53:49
 *  Last Modified:  2011-05-27 16:53:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include "common/odbc/ODBCException.h"

const std::string ODBCException::m_name = "ODBCException";

ODBCException::ODBCException()
{

}

ODBCException::ODBCException(
    const std::string& msg ) :
    Exception( msg )
{

}

ODBCException::ODBCException(
    const std::string& filename,
    int linenum ) :
    Exception( filename, linenum )
{

}

ODBCException::ODBCException(
    const std::string& msg,
    const std::string& filename,
    int linenum ) :
    Exception( msg, filename, linenum )
{

}

std::string
ODBCException::name() const
{
    return m_name;
}


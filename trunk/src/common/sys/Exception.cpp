/*
 * =============================================================================
 *
 *       Filename:  Exception.cpp
 *
 *    Description:  异常封装
 *
 *        Version:  1.0
 *        Created:  2011-05-26 19:10:26
 *  Last Modified:  2011-05-26 19:10:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =============================================================================
 */

#include <sstream>
#include <cstdlib>

#include "common/sys/Exception.h"

////////////////////////////////////////////////////////////////////////////////
// Exception - BEGIN -
////////////////////////////////////////////////////////////////////////////////

Exception::Exception() :
    m_line( 0 )
{

}

Exception::Exception( const std::string& msg ) :
    m_message( msg )
{

}

Exception::Exception(
    const std::string& filename,
    int linenum ) :
    m_file( filename ),
    m_line( linenum )
{

}

Exception::Exception(
    const std::string& msg,
    const std::string& filename,
    int linenum ) :
    m_message( msg ),
    m_file( filename ),
    m_line( linenum )
{

}

Exception::~Exception() throw()
{
}

const std::string Exception::m_name = "Exception";

void
Exception::print( std::ostream& out ) const
{
    out << name() << " (IN FILE "
        << m_file << ", LINE "
        << m_line << ") "
        << m_message;
}

std::string
Exception::name() const
{
    return m_name;
}

const char*
Exception::what() const throw()
{
    try
    {
        if ( m_whatMessage.empty() )
        {
            std::stringstream s;
            print( s );
            m_whatMessage = s.str();
        }

        return m_whatMessage.c_str();
    }
    catch ( ... )
    {

    }

    return "";
}

std::string
Exception::file() const
{
    return m_file;
}

int
Exception::line() const
{
    return m_line;
}

std::string
Exception::message() const
{
    return m_message;
}

std::ostream&
operator << ( std::ostream& out, const Exception& ex )
{
    out << ex.what();

    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Exception - END -
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// NullPointerException - BEGIN -
////////////////////////////////////////////////////////////////////////////////

NullPointerException::NullPointerException()
{

}

NullPointerException::NullPointerException(
    const std::string& msg ) :
    Exception( msg )
{

}

NullPointerException::NullPointerException(
    const std::string& filename,
    int linenum ) :
    Exception( filename, linenum )
{

}

NullPointerException::NullPointerException(
    const std::string& msg,
    const std::string& filename,
    int linenum ) :
    Exception( msg, filename, linenum )
{

}

NullPointerException::~NullPointerException() throw()
{

}

const std::string NullPointerException::m_name = "NullPointerException";

std::string
NullPointerException::name() const
{
    return m_name;
}

////////////////////////////////////////////////////////////////////////////////
// NullPointerException - END -
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// IllegalArgumentException - BEGIN -
////////////////////////////////////////////////////////////////////////////////

IllegalArgumentException::IllegalArgumentException()
{

}

IllegalArgumentException::IllegalArgumentException(
    const std::string& msg ) :
    Exception( msg )
{

}

IllegalArgumentException::IllegalArgumentException(
    const std::string& filename,
    int linenum ) :
    Exception( filename, linenum )
{

}

IllegalArgumentException::IllegalArgumentException(
    const std::string& msg,
    const std::string& filename,
    int linenum ) :
    Exception( msg, filename, linenum )
{

}

IllegalArgumentException::~IllegalArgumentException() throw()
{
}

const std::string IllegalArgumentException::m_name = "IllegalArgumentException";

std::string
IllegalArgumentException::name() const
{
    return m_name;
}

////////////////////////////////////////////////////////////////////////////////
// IllegalArgumentException - END -
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// SyscallException - BEGIN -
////////////////////////////////////////////////////////////////////////////////

SyscallException::SyscallException( int error ) :
    m_error( error )
{

}

SyscallException::SyscallException(
    const std::string& msg,
    int error ) :
    Exception( msg ),
    m_error( error )
{

}

SyscallException::SyscallException(
    const std::string& filename,
    int linenum,
    int error ) :
    Exception( filename, linenum ),
    m_error( error )
{

}

SyscallException::SyscallException(
    const std::string& msg,
    const std::string& filename,
    int linenum,
    int error ) :
    Exception( msg, filename, linenum ),
    m_error( error )
{

}

const std::string SyscallException::m_name = "SyscallException";

std::string
SyscallException::name() const
{
    return m_name;
}

int
SyscallException::getErrorNo() const
{
    return m_error;
}

////////////////////////////////////////////////////////////////////////////////
// SyscallException - END -
////////////////////////////////////////////////////////////////////////////////


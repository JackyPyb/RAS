/*
 * =====================================================================================
 *
 *       Filename:  Tool.h
 *
 *    Description:  for use
 *    *
 *        Version:  1.0
 *        Created:  2010年03月19日 14时14分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (fanbo), duanhancong@uestc.edu.cn
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef TOOL_H_
#define TOOL_H_

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>

class Tool
{
public:
    static unsigned int strToInt( const std::string& sValue );
    static std::string intToStr( unsigned int iValue );
    static unsigned long long strToLong( const std::string& sValue );
    static std::string longToStr( unsigned long long iValue );
    static float strToFloat( const std::string& sValue );
    static std::string floatToStr( float fValue );
    static std::string getTableName( int cmd );
    // the parameters in the data is separeted by |
    static int parseData( char* data, std::vector<std::string>&, char spc );
    static std::string replacePath( std::string oldPath, const std::string& newPath );

    static int  setThreadMask( void ); //added by pxy 2010/07/22, for setting thread signal mask
    static int  getCurrentDateStr(
        std::string& dateStr,  //returns: YYYYMMDD
        time_t tTemp = -1 );  //added by pxy 2010/08/23, for getting current date string
    static time_t getTimeStamp( const std::string& date ); //date: YYYY-m_list-DD HH:m_list:SS
    static int  getCurrentTimeStr( std::string& timeStr, time_t tTemp = -1 ); //returns: YYYY-m_list-DD hh:m_list:ss
    static int  calculateChkSum( const std::string& value );
    static int  replaceDU( std::string& srcDU );
    static int  getTimeValue( const std::string& date );
};

template<typename S, typename D>
D convertType( const S& src )
{
    std::stringstream ss;
    D dst;

    ss << src;
    ss >> dst;
    return dst;
}

#endif

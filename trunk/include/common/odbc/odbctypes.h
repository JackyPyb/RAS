/*
 * =====================================================================================
 *
 *       Filename:  odbctypes.h
 *
 *    Description:   ODBC类型标识及相关C结构
 *
 *        Version:  1.0
 *        Created:  2011-05-07 04:01:01
 *  Last Modified:  2011-05-07 04:01:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shi Wei (sw), shiwei2012@gmail.com
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef COMMON_ODBC_ODBCTYPES_H_
#define COMMON_ODBC_ODBCTYPES_H_

#include <sql.h>
#include <sqlext.h>

/**
 * @brief 类型标识
 */
enum _ODBC_TYPE
{
    ODBC_STRING = 0,
    ODBC_SSHORT = 1,
    ODBC_USHORT = 2,
    ODBC_SINT = 3,
    ODBC_UINT = 4,
    ODBC_FLOAT = 5,
    ODBC_DOUBLE = 6,
    ODBC_BIT = 7,
    ODBC_STINYINT = 8,
    ODBC_UTINYINT = 9,
    ODBC_SBIGINT = 10,
    ODBC_UBIGINT = 11,
    ODBC_BINARY = 12,
    ODBC_DATE = 13,
    ODBC_TIME = 14,
    ODBC_TIMESTAMP = 15,
    ODBC_NUMERIC = 16,
    ODBC_GUID = 17
};

typedef enum _ODBC_TYPE ODBCType;

/**
 * @brief ODBCType到C Data Type的映射
 */
const SQLSMALLINT ODBCCTypes[] =
{
    SQL_C_CHAR,
    SQL_C_SSHORT,
    SQL_C_USHORT,
    SQL_C_SLONG,
    SQL_C_ULONG,
    SQL_C_FLOAT,
    SQL_C_DOUBLE,
    SQL_C_BIT,
    SQL_C_STINYINT,
    SQL_C_UTINYINT,
    SQL_C_SBIGINT,
    SQL_C_UBIGINT,
    SQL_C_BINARY,
    SQL_C_TYPE_DATE,
    SQL_C_TYPE_TIME,
    SQL_C_TYPE_TIMESTAMP,
    SQL_C_NUMERIC,
    SQL_C_GUID
};

/**
 * @brief ODBCType到SQL Data Type的映射
 */
const SQLSMALLINT ODBCSQLTypes[] =
{
    SQL_VARCHAR,
    SQL_SMALLINT,
    SQL_SMALLINT,
    SQL_INTEGER,
    SQL_INTEGER,
    SQL_FLOAT,
    SQL_DOUBLE,
    SQL_BIT,
    SQL_TINYINT,
    SQL_TINYINT,
    SQL_BIGINT,
    SQL_BIGINT,
    SQL_VARBINARY,
    SQL_TYPE_DATE,
    SQL_TYPE_TIME,
    SQL_TYPE_TIMESTAMP,
    SQL_NUMERIC,
    SQL_GUID
};

/**
 * @brief ODBC 参数输入输出类型
 */
enum _ODBC_PARAM_INPUT_OUTPUT_TYPE
{
    ODBC_PARAM_IN,
    ODBC_PARAM_OUT,
    ODBC_PARAM_INOUT,
    ODBC_PARAM_OUT_STREAM,
    ODBC_PARAM_INOUT_STREAM
};

typedef _ODBC_PARAM_INPUT_OUTPUT_TYPE ODBCParamInputOutputType;

const SQLSMALLINT ODBCPARAMIOTYPES[] =
{
    SQL_PARAM_INPUT,
    SQL_PARAM_OUTPUT,
    SQL_PARAM_INPUT_OUTPUT,
    //SQL_PARAM_OUTPUT_STREAM,
    //SQL_PARAM_INPUT_OUTPUT_STREAM
};

// Fetch完所有数据
const int ODBC_NO_DATA = 0x01;

////////////////////////////// For ENV - BEGIN - ///////////////////////////////
// Connection Pool
#define ODBC_CP_OFF SQL_CP_OFF
#define ODBC_CP_ONE_PER_DRIVER SQL_CP_ONE_PER_DRIVER
#define ODBC_CP_ONE_PER_HENV SQL_CP_ONE_PER_HENV
// ODBC version
#define ODBC_V2 SQL_OV_ODBC2
#define ODBC_V3 SQL_OV_ODBC3
#define ODBC_V3_8 SQL_OV_ODBC3_80
///////////////////////////// For ENV - END - //////////////////////////////////

#endif  // COMMON_ODBC_ODBCTYPES_H_


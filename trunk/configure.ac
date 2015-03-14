#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([RAS], [1.0], [yubingpenguestc@163.com])
AC_CONFIG_SRCDIR([])
AM_INIT_AUTOMAKE(RAS, 1.0)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
LT_INIT
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_ARG_ENABLE(debug, [  --enable-debug	Enable DEBUG output. ],
	[ CXXFLAGS="-g2 -O0 -DDEBUG -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -Wall -Werror" ],
	[ CXXFLAGS="-O2 -DNDEBUG -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -Wall" ])

# Checks for libraries.
AC_CHECK_LIB(pthread, main)
AC_CHECK_LIB(rt, main)
#AC_CHECK_LIB(xml2, main)
#AC_CHECK_LIB(odbc, main)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile 
                 src/Makefile
                 src/common/Makefile
                 src/common/comm/Makefile
                 src/common/daemon/Makefile
                 src/common/log/Makefile
                 src/common/sys/Makefile
                 src/common/Timer/Makefile
                 src/common/util/Makefile
                 src/common/xml/Makefile
                 src/protocol/Makefile
                 src/testserver/Makefile
                 src/testclient/Makefile
                 src/RC/Makefile
                 test/Makefile
                 ])
AC_OUTPUT

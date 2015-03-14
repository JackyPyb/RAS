INCLUDES=-I$(top_srcdir)/include 
noinst_LTLIBRARIES=libodbc.la
SUBDIRS=
libodbc_la_SOURCES=ConnectionFactory.cpp \
				   ConnectionImpl.cpp \
				   ConnectionPool.cpp \
				   ODBCBindColumnException.cpp \
				   ODBCBindParameterException.cpp \
				   odbcerr.cpp \
				   ODBCException.cpp \
				   Statement.cpp

LIBS += -lodbc

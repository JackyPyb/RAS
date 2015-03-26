AUTOMAKE_OPTIONS = foreign
INCLUDES = -I$(top_srcdir)/include -I/usr/include/libxml2

bin_PROGRAMS = $(top_srcdir)/bin/AL
__top_srcdir__bin_AL_SOURCES = main.cpp									 
__top_srcdir__bin_AL_LDADD = $(top_builddir)/src/AL/libal.la


noinst_LTLIBRARIES = $(top_builddir)/src/AL/libal.la
__top_builddir__src_AL_libal_la_SOURCES = \
										  ServerAgent.h \
										  ServerAgent.cpp \
										  ConfigManager.h \
										  ConfigManager.cpp
__top_builddir__src_AL_libal_la_LIBADD = \
									$(top_srcdir)/lib/libcommon.la \
			                     	$(top_srcdir)/lib/libprotocol.la

LIBS += -lprotobuf -lxml2
			                     	

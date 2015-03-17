AUTOMAKE_OPTIONS = foreign
INCLUDES = -I$(top_srcdir)/include  -I$(top_srcdir)/include/RC -I/usr/include/libxml2

bin_PROGRAMS = $(top_srcdir)/bin/RC
__top_srcdir__bin_RC_SOURCES = main.cpp									 
__top_srcdir__bin_RC_LDADD = $(top_builddir)/src/RC/librc.la


noinst_LTLIBRARIES = $(top_builddir)/src/RC/librc.la
__top_builddir__src_RC_librc_la_SOURCES = \
										ConfigManager.h \
										ConfigManager.cpp \
										RunControl.h \
										RunControl.cpp \
										Strategy.h \
										StrategyManager.h \
										StrategyManager.cpp \
										Task.h \
										GetNCFromAll.h \
										GetNCFromAll.cpp \
										GetNCFromOne.h \
										GetNCFromOne.cpp \
										NCAgent.h \
										NCAgent.cpp \
										InfoFromNCToRC.h \
										InfoFromNCToRC.cpp \
										ResourceScheduler.h \
										ResourceScheduler.cpp \
										InfoSendToNC.h \
										InfoSendToNC.cpp \
										NCLoadBalance.h \
										NCLoadBalance.cpp \
										ResourceManager.h \
										ResourceManager.cpp \
										FWInstance.h \
										FWInstance.cpp \
										FWIManager.h \
										FWIManager.cpp \
										SASAgent.h \
										SASAgent.cpp \
										ALAgent.h \
										ALAgent.cpp \
										FWMAgent.h \
										FWMAgent.cpp \
										InfoFromFWMToRC.h \
										InfoFromFWMToRC.cpp \
										InfoSendToFWM.h \
										InfoSendToFWM.cpp \
										ContainResourceTask.h \
										NCHeartBeatTimer.h \
										NCHeartBeatTimer.cpp \
										FWMHeartBeatTimer.h \
										FWMHeartBeatTimer.cpp \
										NCRegTask.h \
										NCRegTask.cpp
__top_builddir__src_RC_librc_la_LIBADD = \
									$(top_srcdir)/lib/libcommon.la \
			                     	$(top_srcdir)/lib/libprotocol.la

LIBS += -lprotobuf -lxml2
			                     	

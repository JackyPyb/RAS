INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/include/protocol
noinst_LTLIBRARIES=$(top_srcdir)/lib/libprotocol.la
SUBDIRS=
BUILT_SOURCES=$(PROTOOUTPUT)
nodist___top_srcdir__lib_libprotocol_la_SOURCES=$(PROTOOUTPUT) 

PROTOOUTPUT=AlProtocol.pb.cc \
			AlProtocol.pb.h \
			FwmRcProtocol.pb.cc \
			FwmRcProtocol.pb.h \
			RcNcProtocol.pb.cc \
			RcNcProtocol.pb.h \
			RcSasProtocol.pb.cc \
			RcSasProtocol.pb.h

PROTOINPUT=AlProtocol.proto \
		   FwmRcProtocol.proto \
		   RcNcProtocol.proto \
		   RcSasProtocol.proto 

EXTRA_DIST=$(PROTOINPUT)

PROTODIR=$(top_srcdir)/src/protocol

$(PROTOOUTPUT):$(PROTOINPUT)
	protoc -I$(PROTODIR) --cpp_out=$(PROTODIR) $^
	mv $(PROTODIR)/*.pb.h $(top_srcdir)/include/protocol

clean:
	-rm -f $(top_srcdir)/include/protocol/*.pb.h
	-rm -f $(PROTODIR)/*.pb.cc
	-rm -f *.lo
	-rm -f *.o

distclean: clean
	-rm -f Makefile

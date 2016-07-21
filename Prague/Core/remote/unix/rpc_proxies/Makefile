ROOT:=../../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=proxies

INCLUDES= \
	$(ROOT)/include/Prague \
	$(ROOT)/include \
	$(ROOT)/CommonFiles \
	../ \
	../rpc_headers \
	../../../../thpoolimp

DEFINES= \
	 FRAME_TRACE

LIBRARIES= \
	rpc \
	hashutil

DEPS= \
	../rpc \
	$(ROOT)/CommonFiles/hashutil

STDLIBS= $(THREADSLIB)

SOURCES= \
	remote_proxy.cpp

include $(ROOT)/Makefiles/Make.Archive

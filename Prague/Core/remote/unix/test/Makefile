ROOT:=../../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=prtest

INCLUDES= \
	. \
	rpc_headers \
	rpc_headers/pr_system \
	../ \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles

LIBRARIES= \
	remote

DEPS= \
	../

STDLIBS= $(THREADSLIB) $(DSOLIB)

SOURCES= \
	test.cpp

include $(ROOT)/Makefiles/Make.Executable

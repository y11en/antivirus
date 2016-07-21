ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=remote

INCLUDES= \
	. \
	rpc_headers \
	rpc_headers/pr_system \
	../ \
	../../../thpoolimp \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles


DEFINES= \
	_USE_VTBL

LIBRARIES= \
	rpc \
	hashutil

DEPS= \
	rpc \
	$(ROOT)/CommonFiles/hashutil

STDLIBS= $(THREADSLIB) $(DSOLIB)

SOURCES= \
	rpc_connect_unix.cpp \
	../pr_remote.cpp \
	../pr_internal.cpp \
	../pr_root_c.cpp \
	../pr_system/pr_system_c.cpp \
	../SharedTable.cpp

include $(ROOT)/Makefiles/Make.Library

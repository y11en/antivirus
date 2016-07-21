ROOT:=../../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=rpc

SOURCES= \
	../../../../thpoolimp/thpimpl_unix.cpp \
	uuid.cpp \
	connectionmanager.cpp \
	request.cpp \
	requestmanager.cpp \
	praguerpc.cpp

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles \
	../../../../thpoolimp

include $(ROOT)/Makefiles/Make.Archive

ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=meta2atem

INCLUDES= \
	../ \
	$(ROOT)/CommonFiles \
	$(ROOT)/include \
	$(ROOT)/include/Prague

SOURCES= \
	meta2atem.cpp

LIBRARIES= \
	metadata \
	klsys

DEPS= \
	../metadata \
	$(ROOT)/CommonFiles/klsys

HOSTONLY=1
export HOSTONLY

include $(ROOT)/Makefiles/Make.Executable

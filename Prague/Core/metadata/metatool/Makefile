ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=metatool

INCLUDES= \
	../ \
	$(ROOT)/CommonFiles \
	$(ROOT)/include \
	$(ROOT)/include/Prague

SOURCES= \
	metatool.cpp

LIBRARIES= \
	metadata \
	klsys

DEPS= \
	../metadata \
	$(ROOT)/CommonFiles/klsys

HOSTONLY=1
export HOSTONLY

include $(ROOT)/Makefiles/Make.Executable

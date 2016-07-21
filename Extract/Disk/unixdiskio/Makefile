ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=unixdiskio

INCLUDES= \
	$(ROOT)/CommonFiles \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/Extract

SOURCES= \
	io.c \
	objptr.c \
	plugin_unixdiskio.c \
	diskio.cpp

include $(ROOT)/Makefiles/Make.Plugin

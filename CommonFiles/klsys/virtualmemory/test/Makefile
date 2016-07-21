ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=vm_test

INCLUDES = \
	$(ROOT)/include \
	$(ROOT)/CommonFiles

SOURCES= \
	virtualmemory_test.cpp \
	../virtualmemory_unix.cpp \
	../virtualmemory_win32.cpp

include $(ROOT)/Makefiles/Make.Executable

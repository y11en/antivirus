ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=nfio

INCLUDES= \
	$(ROOT)/CommonFiles \
	$(ROOT)/include/Prague \
	$(ROOT)/include

SOURCES= \
	plugin_nfio.cpp \
	unix_io.cpp \
	unix_objptr.cpp \
	unix_os.cpp \
	unix_utils.cpp

include $(ROOT)/Makefiles/Make.Plugin

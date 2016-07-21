ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=klavsys

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/klava \
	$(ROOT)/include/klava/bases \
	$(ROOT)/CommonFiles \

LIBRARIES= \
	klavcorelib \
	klsys \
	lzmalib

DEPS= \
	../klavcorelib \
	$(ROOT)/CommonFiles/klsys \
	$(ROOT)/external/lzmalib

STDLIBS=$(THREADSLIB) $(DSOLIB)

SOURCESLIST=Sources.List

VCPROJ=klavsys.vcproj

include $(ROOT)/Makefiles/Make.Library


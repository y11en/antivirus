ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=klav_engine

TARG=$(BINDIR)/$(TARGET).$(SO)

INCLUDES= \
	$(ROOT)/include

LIBRARIES= \
	klavsyslib \
	klavcorelib \
	klsys \
	lzmalib

DEPS= \
	../klavsyslib \
	../klavcorelib \
	$(ROOT)/CommonFiles/klsys \
	$(ROOT)/external/lzmalib


STDLIBS=$(THREADSLIB) $(DSOLIB)

SOURCESLIST=Sources.List

VCPROJ=klav_engine.vcproj

include $(ROOT)/Makefiles/Make.Library

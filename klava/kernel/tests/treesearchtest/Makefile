ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=treesearchtest

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles

LIBRARIES= \
	klsys \
	treesearchlib \
	klavcorelib

DEPS= \
	$(ROOT)/CommonFiles/klsys \
	../../treesearchlib \
	../../klavcorelib

VCPROJ=treesearchtest.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Executable


ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=metainfo

INCLUDES= \
	$(ROOT)/include/ \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles

LIBRARIES= \
	metadata \
	klsys

DEPS= \
	../metadata \
	$(ROOT)/CommonFiles/klsys

SOURCESLIST=Sources.List

VCPROJ=metainfo.vcproj

export HOSTONLY=1

include $(ROOT)/Makefiles/Make.Executable

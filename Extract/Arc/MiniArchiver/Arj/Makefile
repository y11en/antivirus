ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=arj

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/Extract \
	$(ROOT)/CommonFiles \
	$(ROOT)/Prague

LIBRARIES= \
	hashutil

DEPS= \
	$(ROOT)/CommonFiles/hashutil

METAFILE=arj.$(META)

SOURCESLIST=Sources.List

VCPROJ=arj.vcproj

include $(ROOT)/Makefiles/Make.Plugin

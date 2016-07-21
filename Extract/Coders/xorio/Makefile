ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=xorio

INCLUDES= \
	$(ROOT)/include/Prague \
	$(ROOT)/include/Extract \
	$(ROOT)/include \
	$(ROOT)/CommonFiles

LIBRARIES= \
	hashutil

DEPS= \
	$(ROOT)/CommonFiles/hashutil

METAFILE=xorio.$(META)

SOURCESLIST=Sources.List

VCPROJ=xorio.vcproj

include $(ROOT)/Makefiles/Make.Plugin

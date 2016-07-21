ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=minizip

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/Extract \
	$(ROOT)/CommonFiles \
	$(ROOT)/Extract/Arc/Pack \
	$(ROOT)/Extract/Wrappers \
	$(ROOT)/Prague

LIBRARIES= \
	hashutil

DEPS= \
	$(ROOT)/CommonFiles/hashutil

METAFILE=minizip.$(META)

SOURCESLIST=Sources.List

VCPROJ=minizip.vcproj

include $(ROOT)/Makefiles/Make.Plugin

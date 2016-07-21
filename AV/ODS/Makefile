ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=ods

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/AV \
	$(ROOT)/include/ProductCore \
	$(ROOT)/include/Extract \
	$(ROOT)/Extract/Memory \
	$(ROOT)/CommonFiles

DEFINES= \
	DIRECT_SEND_EVENTS

LIBRARIES= \
	hashutil

DEPS= \
	../../CommonFiles/hashutil

METAFILE=ods.$(META)

VCPROJ=ods.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Plugin

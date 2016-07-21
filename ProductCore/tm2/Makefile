ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=tm

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/ProductCore \
	$(ROOT)/include/AV \
	$(ROOT)/CommonFiles \
	$(ROOT)/CommonFiles/licensing/Include

LIBRARIES= \
	hashutil

DEPS= \
	../../CommonFiles/hashutil

METAFILE=tm.$(META)

VCPROJ=tm.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Plugin

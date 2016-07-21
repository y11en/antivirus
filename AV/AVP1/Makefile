ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=avp1

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/AV \
	$(ROOT)/include/Extract \
	$(ROOT)/AVP32 \
	$(ROOT)/CommonFiles

DEFINES= \
	BASEWORK_4_PRAGUE

LIBRARIES= \
	basework \
	serialize \
	property \
	swmanager \
	compatutils \
	lzmalib

DEPS= \
	$(ROOT)/AVP32/BaseWork \
	$(ROOT)/CommonFiles/Serialize \
	$(ROOT)/CommonFiles/Property \
	$(ROOT)/CommonFiles/SWManager \
	$(ROOT)/CommonFiles/Sign \
	$(ROOT)/CommonFiles/unix/compatutils \
	$(ROOT)/external/lzmalib

METAFILE=Avp1.$(META)

SOURCESLIST=Sources.List

VCPROJ=avp1.vcproj

STDLIBS=$(THREADSLIB)

include $(ROOT)/Makefiles/Make.Plugin

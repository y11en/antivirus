ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=avp3info

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/AV \
	$(ROOT)/include/Extract \
	$(ROOT)/include/ProductCore \
	$(ROOT)/CommonFiles \
	$(ROOT)/CommonFiles/Stuff \
	$(ROOT)/AVP32

METAFILE=avp3info.$(META)

SOURCESLIST=Sources.List

VCPROJ=avp3info.vcproj

LIBRARIES=\
	klavprlib \
	klavcorelib

DEPS=\
	$(ROOT)/klava/kernel/klavprlib \
	$(ROOT)/klava/kernel/klavcorelib

include $(ROOT)/Makefiles/Make.Plugin

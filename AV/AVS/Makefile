ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=avs

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/AV \
	$(ROOT)/include/ProductCore \
	$(ROOT)/include/Extract \
	$(ROOT)/Extract/Memory \
	$(ROOT)/CommonFiles \
	$(ROOT)/klava/kernel/include \
	$(ROOT)/Prague \
	$(ROOT)/ProductCore \
	$(ROOT)/Components/KeepUp2Date/Client

LIBRARIES= \
	klavprlib \
	klavcorelib \
	lzmalib \
	compatutils

DEPS= \
	$(ROOT)/klava/kernel/klavprlib \
	$(ROOT)/klava/kernel/klavcorelib \
	$(ROOT)/external/lzmalib \
	$(ROOT)/CommonFiles/unix/compatutils

METAFILE=avs.$(META)

VCPROJ=avs.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Plugin

ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=prkernel

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles

LIBRARIES= \
	hashutil

DEPS= \
	$(ROOT)/CommonFiles/hashutil

METAFILE=kernel.$(META)

SOURCESLIST=Sources.List

VCPROJ=Kernel.vcproj

include $(ROOT)/Makefiles/Make.Plugin

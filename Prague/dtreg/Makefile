ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=dtreg

INCLUDES= \
	$(ROOT)/include/Prague \
	$(ROOT)/include \
	$(ROOT)/CommonFiles

LIBRARIES= \
	serialize \
	swmanager \
	hashutil \
	property

DEPS= \
	$(ROOT)/CommonFiles/Property \
	$(ROOT)/CommonFiles/Serialize \
	$(ROOT)/CommonFiles/SWManager \
	$(ROOT)/CommonFiles/hashutil


METAFILE=dtreg.$(META)

SOURCESLIST=Sources.List

VCPROJ=dtreg.vcproj

include $(ROOT)/Makefiles/Make.Plugin

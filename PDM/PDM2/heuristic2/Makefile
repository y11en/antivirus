ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=pdm2

INCLUDES= \
	$(ROOT)/include

ifneq ($(DEBUG),0)
DEFINES+= \
	_DEBUG
endif

VCPROJ=pdm2.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Archive

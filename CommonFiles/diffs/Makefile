ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=diffs

INCLUDES= \
	../ \
	$(ROOT)/external \
	$(ROOT)/include/klava
\
DEFINES= \
	_LIB \
	KLAVPACK_EXPORTS

LIBRARIES= \
	lzmalib

DEPS= \
	../../external/lzmalib

VCPROJ=diffs.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Library

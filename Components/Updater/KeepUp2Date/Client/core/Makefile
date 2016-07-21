ROOT:=../../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=updater

INCLUDES= \
	$(ROOT)/external/boost \
	$(ROOT)/external \
	$(ROOT)/CommonFiles \
	"$(ROOT)/CS AdminKit/development2/include" \



DEFINES= \
	CORE_EXPORTS \
	KAVUPDCORE_EXPORTS \
	KLUPD_DISABLE_SIGNATURE_5 \
#	_USE_32BIT_TIME_T

LIBRARIES= \
	dskm \
	SWManager \
	diffs \
	lzmalib \
	property \
	cryptoc \
	

DEPS= \
	

VCPROJ=Updater.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Library

ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=avpgui

INCLUDES= \
	../Include \
	../../prague/Include \
	../../prague \
	../../CommonFiles \
	../../Updater60/Include

DEFINES= \
	XTREE_MEM_PRAGUE \

LIBRARIES= \
	compatutils

DEPS= \
	../../prague/unix/compatutils

METAFILE=avpgui.$(META)

VCPROJ=avpgui.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Plugin

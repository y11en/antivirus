ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=bl

INCLUDES= \
	../Include \
	../../prague/Include \
	../../CommonFiles \
	../../Updater60/Include

METAFILE=bl.$(META)

SOURCES= \
	bl.cpp \
	blloc.cpp \
	plugin_bl.cpp \
	update.cpp

VCPROJ=bl.vcproj

#SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Plugin

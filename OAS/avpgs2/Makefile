ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=avpgs2

INCLUDES= \
	${ROOT}/include \
	${ROOT}/CommonFiles

DEPS= \
	

METAFILE=avpgs2.$(META)
SOURCES= \
	avpg2.cpp \
	plugin_avpgs2.cpp \
	mavpg.cpp \
	mac_avpgs2.c \
	kliflib/kliflib.cpp
	
include $(ROOT)/Makefiles/Make.Plugin

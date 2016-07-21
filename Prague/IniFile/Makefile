ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=inifile

INCLUDES= \
	$(ROOT)/include/Prague \
	$(ROOT)/include \
	$(ROOT)/CommonFiles

LIBRARIES= \
	compatutils

DEPS= \
	$(ROOT)/CommonFiles/unix/compatutils

METAFILE=IniFile.$(META)

SOURCESLIST=Sources.List

VCPROJ=inifile.vcproj

include $(ROOT)/Makefiles/Make.Plugin

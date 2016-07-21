ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=klaveng_test

INCLUDES= \
	$(ROOT)/include

LIBRARIES=\
	klavsys \
	klavcorelib \
	klsys

DEPS=\
	../../klavsys \
	../../klavcorelib \
	$(ROOT)/CommonFiles/klsys

STDLIBS=$(DSOLIB)

VCPROJ=klaveng_test.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Executable


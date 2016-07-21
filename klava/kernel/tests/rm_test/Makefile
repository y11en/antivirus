ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=rm_test

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/CommonFiles

LIBRARIES= \
	klavsyslib \
	klavcorelib \
	klavenglib \
	klsys \

DEPS= \
	../../klavsyslib \
	../../klavcorelib \
	$(ROOT)/klava/bases/klavenglib \
	$(ROOT)/CommonFiles/klsys

VCPROJ=rm_test.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Executable


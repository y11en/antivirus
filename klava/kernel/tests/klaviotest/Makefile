ROOT:=../../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=klaviotest

INCLUDES= \
	$(ROOT)/include

LIBRARIES= \
	klsys \
	klavsyslib \
	klavenglib \
	klavcorelib

DEPS= \
	$(ROOT)/CommonFiles/klsys \
	../../klavsyslib \
	$(ROOT)/klava/bases/klavenglib \
	../../klavcorelib

VCPROJ=klaviotest.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Executable


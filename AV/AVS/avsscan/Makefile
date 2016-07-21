ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=avsscan

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/AV \
	$(ROOT)/include/ProductCore \
	$(ROOT)/CommonFiles

LIBRARIES= \
	klavcorelib \
	klavsyslib \
	klsys

DEPS= \
	$(ROOT)/klava/kernel/klavcorelib \
	$(ROOT)/klava/kernel/klavsyslib \
	$(ROOT)/CommonFiles/klsys

STDLIBS=$(DSOLIB)

VCPROJ=avsscan.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Executable

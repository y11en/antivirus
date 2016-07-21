ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=kav50

INCLUDES= \
	../Include \
	../../prague/Include \
	../../Updater60/Include \
	../../CommonFiles

LIBRARIES= \
	remote \
	hashutil \
	compatutils

DEPS= \
	../../prague/remote/unix \
	../../CommonFiles/hashutil \
	../../prague/unix/compatutils

STDLIBS= $(THREADSLIB) $(DSOLIB)

VCPROJ=kav50.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Executable

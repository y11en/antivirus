ROOT:=../..

include $(ROOT)/Makefiles/Make.Config

TARGET=basegui
TARG=$(BINDIR)/$(TARGET).ppl

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/ProductCore \
	$(ROOT)/include/Extract \
	$(ROOT)/CommonFiles \
	$(QTINCDIR) \
	$(QTINCDIR)/Qt \
	$(QTINCDIR)/QtGui \
	$(QTINCDIR)/QtCore


LIBRARIES= \
	unigui \
	compatutils \
	hashutil

ifeq ($(SYSTARG),Darwin)
LDFLAGS+=-lz -framework QtCore -framework QtGui -F$(QTLIBDIR)
else
LIBRARIES+=\
	QtCore \
	QtGui
LIBSDIR= $(QTLIBDIR)
endif


DEPS= \
	unigui \
	$(ROOT)/CommonFiles/unix/compatutils 

METAFILE=basegui.meta

STDLIBS=$(THREADSLIB)
VCPROJ=basegui.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Plugin

ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=unigui

INCLUDES= \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/include/ProductCore \
	$(ROOT)/CommonFiles \
	$(QTINCDIR) \
	$(QTINCDIR)/Qt \
	$(QTINCDIR)/QtGui \
	$(QTINCDIR)/QtCore

VCPROJ=unigui.vcproj

SOURCESLIST=Sources.List

include $(ROOT)/Makefiles/Make.Archive

UniCtrls.cpp: moc_ctrls.cpp

moc_ctrls.cpp: UniCtrls.h
	@echo "***** Generating $@"
	$(QTMOC) $< -o $@

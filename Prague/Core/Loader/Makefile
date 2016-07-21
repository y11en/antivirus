ROOT:=../../..

include $(ROOT)/Makefiles/Make.Config

TARGET=loader
EXTENTION=$(SO)
TARG=$(BINDIR)/$(TARGET).$(EXTENTION)

INCLUDES= \
	./ \
	./unix \
	$(ROOT)/include \
	$(ROOT)/include/Prague \
	$(ROOT)/CommonFiles

DEFINES= \
	ITS_LOADER \
	LATE_CHECK

LIBRARIES= \
        hashutil \
        klsys \
        metadata \
        dskm \
        cryptoc \
        property \
        serialize \
        swmanager \
        compatutils

DEPS= \
	$(ROOT)/CommonFiles/SWManager \
	$(ROOT)/CommonFiles/Serialize \
	$(ROOT)/CommonFiles/Property \
	$(ROOT)/CommonFiles/klsys \
	../metadata/metadata \
	$(ROOT)/CommonFiles/Dskm \
	$(ROOT)/CommonFiles/Dskm/Gost/CryptoC \
	$(ROOT)/CommonFiles/hashutil \
	$(ROOT)/CommonFiles/unix/compatutils 


STDLIBS=$(THREADSLIB) $(DSOLIB)

METAFILE=Loader.$(META)

SOURCESLIST=Sources.List

VCPROJ=loader.vcproj

EXPORTS=unix/exports.$(OBJFORMAT)

include $(ROOT)/Makefiles/Make.Plugin

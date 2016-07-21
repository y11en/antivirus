.SUFFIXES: .c .cpp .h .idl .exe .obj

BUILDDIR=build
BINDIR=bin

IDLC=..\Debug\bin\idlcpp.exe -Isystem

PRAGUE_FLAGS= -D_PRAGUE_TRACE_ -I/Include -I/include/prague -Iprague

CMMNFLAGS= -nologo -Zi -MTd $(PRAGUE_FLAGS) -I$(BUILDDIR) -Isystem
CFLAGS= -TC $(CMMNFLAGS)
CPPFLAGS= -TP -EHs $(CMMNFLAGS)

#{.}.idl{$(BUILDDIR)}.idl:
#	cp $< $@

{.}.c{$(BUILDDIR)}.obj:
	cl -c $(CFLAGS) $< /Fo$(BUILDDIR)/$(@F)
.c.obj:
	cl -c $(CFLAGS) $< /Fo$(BUILDDIR)/$(@F)

#{.}.cpp{$(BUILDDIR)}.obj:
#	cl -c $(CPPFLAGS) $< /Fo$(BUILDDIR)/$(@F)
#.cpp.obj:
#	cl -c $(CPPFLAGS) $< /Fo$(BUILDDIR)/$(@F)

{.}.cpp{$(BUILDDIR)}.obp:
	cl -c $(CPPFLAGS) $< /Fo$(BUILDDIR)/$(@F)
.cpp.obp:
	cl -c $(CPPFLAGS) $< /Fo$(BUILDDIR)/$(@F)

.idl{$(BUILDDIR)}.h:
	$(IDLC) -I. $< --output-dir $(BUILDDIR)

all : $(BINDIR)/sampleplugin.ppl $(BINDIR)/sampleplugincpp.ppl $(BINDIR)/client.exe $(BINDIR)/clientcpp.exe
#all : $(BINDIR)/sampleplugincpp.ppl $(BINDIR)/client.exe $(BINDIR)/clientcpp.exe

$(BUILDDIR)/sampleplugin.h: sampleplugin.idl

$(BUILDDIR)/sampleplugin.obj: $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h
#$(BUILDDIR)/sampleplugin.obp: $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h
$(BUILDDIR)/sampleimpl.obp: $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h $(BUILDDIR)/sampleimpl.h
$(BUILDDIR)/sampleimpl.obj: $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h $(BUILDDIR)/sampleimpl.h
$(BUILDDIR)/userimpl.obp: $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h
$(BUILDDIR)/userimplcpp.obp: $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h
$(BUILDDIR)/client.obp : $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h 
$(BUILDDIR)/clientcpp.obp : $(BUILDDIR)/sampleplugin.h $(BUILDDIR)/sample.h 

$(BINDIR)/sampleplugin.ppl : $(BUILDDIR)/sampleplugin.obj $(BUILDDIR)/userimpl.obp
	link /nologo /out:$@ /dll /debug $**
#	copy /b $@ + $(BUILDDIR)/sampleplugin.metadata  $@

$(BINDIR)/sampleplugincpp.ppl : $(BUILDDIR)/sampleplugin.obj $(BUILDDIR)/userimplcpp.obp $(BUILDDIR)/sampleimpl.obp
	link /nologo /out:$@ /dll /debug $**
#	cat $(BUILDDIR)/sampleplugin.metadata >> $@

$(BINDIR)/client.exe : $(BUILDDIR)/client.obp
	link /nologo /out:$@ /debug $**
		
$(BINDIR)/clientcpp.exe : $(BUILDDIR)/clientcpp.obp
	link /nologo /out:$@ /debug $**
		
clean:
	rm -f $(BUILDDIR)/*.* $(BINDIR)/*.*

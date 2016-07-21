# Microsoft Developer Studio Generated NMAKE File, Based on Avp_io VxD.dsp
!IF "$(CFG)" == ""
CFG=Avp_io VxD - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Avp_io VxD - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Avp_io VxD - Win32 Release" && "$(CFG)" != "Avp_io VxD - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp_io VxD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Avp_io VxD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avp_io VxD - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release\avpiovxd
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\AVP_IO.vxd"


CLEAN :
	-@erase "$(INTDIR)\Avp_io.obj"
	-@erase "$(INTDIR)\C_vxdio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVP_IO.pdb"
	-@erase "$(OUTDIR)\AVP_IO.vxd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /GB /Zp1 /MT /W3 /Zi /I "..\..\CommonFiles" /I "..\vtoolsd\include" /I "$(VTOOLSD)\include" /D "NDEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /Zl /FD /Gs /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp_io VxD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib /nologo /incremental:no /pdb:"$(OUTDIR)\AVP_IO.pdb" /debug /machine:IX86 /nodefaultlib /def:".\Avp_io.def" /out:"$(OUTDIR)\AVP_IO.vxd" /libpath:"$(VTOOLSD)\lib" /libpath:"..\..\commonfiles\release" /IGNORE:4078,4039,4070 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA 
LINK32_OBJS= \
	"$(INTDIR)\Avp_io.obj" \
	"$(INTDIR)\C_vxdio.obj"

"$(OUTDIR)\AVP_IO.vxd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\release\AVP_IO.vxd
TargetName=AVP_IO
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AVP_IO.vxd"
   PostBuild.cmd RELEASE \out\release\AVP_IO.vxd AVP_IO
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Avp_io VxD - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug

ALL : "..\..\out\debug\AVP_IO.vxd"


CLEAN :
	-@erase "$(INTDIR)\Avp_io.obj"
	-@erase "$(INTDIR)\C_vxdio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AVP_IO.pdb"
	-@erase "..\..\out\debug\AVP_IO.ilk"
	-@erase "..\..\out\debug\AVP_IO.vxd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /Zp1 /MTd /W3 /Gm /Zi /Od /I "..\..\CommonFiles" /I "..\vtoolsd\include" /I "$(VTOOLSD)\include" /D "DEBUG" /D "_DEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp_io VxD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=icrtms.obj clms4d.lib daamsd.lib ndms95d.lib rtmsd.lib ntmsd.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib chkesp.obj /nologo /incremental:yes /pdb:"$(OUTDIR)\AVP_IO.pdb" /debug /machine:IX86 /nodefaultlib /def:".\Avp_io.def" /out:"../../out/debug/AVP_IO.vxd" /libpath:"$(VTOOLSD)\lib" /libpath:"..\..\commonfiles\release" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA 
LINK32_OBJS= \
	"$(INTDIR)\Avp_io.obj" \
	"$(INTDIR)\C_vxdio.obj"

"..\..\out\debug\AVP_IO.vxd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\debug\AVP_IO.vxd
TargetName=AVP_IO
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\out\debug\AVP_IO.vxd"
   PostBuild.cmd DEBUG \out\debug\AVP_IO.vxd AVP_IO
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Avp_io VxD.dep")
!INCLUDE "Avp_io VxD.dep"
!ELSE 
!MESSAGE Warning: cannot find "Avp_io VxD.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Avp_io VxD - Win32 Release" || "$(CFG)" == "Avp_io VxD - Win32 Debug"
SOURCE=.\Avp_io.cpp

"$(INTDIR)\Avp_io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\C_vxdio.cpp

"$(INTDIR)\C_vxdio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Avp_io.rc

!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on w9x.dsp
!IF "$(CFG)" == ""
CFG=w9x - Win32 Debug
!MESSAGE No configuration specified. Defaulting to w9x - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "w9x - Win32 Release" && "$(CFG)" != "w9x - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "w9x.mak" CFG="w9x - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "w9x - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "w9x - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "w9x - Win32 Release"

OUTDIR=.\..\Release
INTDIR=.\Release

ALL : "..\Release9x\klif.vxd"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\VXDHOOK.OBJ"
	-@erase "$(INTDIR)\WINS.OBJ"
	-@erase "..\Release9x\klif.vxd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /Zp1 /MT /W3 /O2 /Oy- /I "$(VTOOLSD)\include" /I "..\..\..\CommonFiles" /D "NDEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /Zl /FD /Gs /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\w9x.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfms.lib /nologo /pdb:none /machine:IX86 /nodefaultlib /def:".\AVPG.DEF" /out:"..\Release9x/klif.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA 
DEF_FILE= \
	".\AVPG.DEF"
LINK32_OBJS= \
	"$(INTDIR)\VXDHOOK.OBJ" \
	"$(INTDIR)\WINS.OBJ"

"..\Release9x\klif.vxd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
   PreBuild.cmd Release
	 $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetName=klif
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\Release9x\klif.vxd"
   PostBuild.cmd Release ..\Release9x\klif.vxd klif
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "w9x - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug

ALL : "..\Debug9x\klif.vxd"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\VXDHOOK.OBJ"
	-@erase "$(INTDIR)\WINS.OBJ"
	-@erase "..\Debug9x\klif.pdb"
	-@erase "..\Debug9x\klif.vxd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /Zp1 /MTd /W3 /Zi /Od /Gf /I "$(VTOOLSD)\include" /I "..\..\..\CommonFiles" /D "DEBUG" /D "_DEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Gs /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\w9x.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=icrtms.obj clms4d.lib daamsd.lib ndms95d.lib rtmsd.lib ntmsd.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfmsd.lib /nologo /incremental:no /pdb:"..\Debug9x/klif.pdb" /debug /machine:IX86 /nodefaultlib /def:".\AVPG.DEF" /out:"..\Debug9x\klif.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA 
DEF_FILE= \
	".\AVPG.DEF"
LINK32_OBJS= \
	"$(INTDIR)\VXDHOOK.OBJ" \
	"$(INTDIR)\WINS.OBJ"

"..\Debug9x\klif.vxd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
   PreBuild.cmd Debug
	 $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetName=klif
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\Debug9x\klif.vxd"
   PostBuild.cmd Debug ..\Debug9x\klif.vxd klif
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
!IF EXISTS("w9x.dep")
!INCLUDE "w9x.dep"
!ELSE 
!MESSAGE Warning: cannot find "w9x.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "w9x - Win32 Release" || "$(CFG)" == "w9x - Win32 Debug"
SOURCE=.\firewall.c
SOURCE=.\i21.c
SOURCE=.\ifs.c
SOURCE=.\io.c
SOURCE=.\ios.c
SOURCE=.\LLDiskIO.c
SOURCE=.\r3.c
SOURCE=.\registry.c
SOURCE=.\VXDHOOK.C

"$(INTDIR)\VXDHOOK.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WINS.CPP

"$(INTDIR)\WINS.OBJ" : $(SOURCE) "$(INTDIR)"



!ENDIF 


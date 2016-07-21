# Microsoft Developer Studio Generated NMAKE File, Based on nt.dsp
!IF "$(CFG)" == ""
CFG=nt - Win32 Debug
!MESSAGE No configuration specified. Defaulting to nt - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "nt - Win32 Retail" && "$(CFG)" != "nt - Win32 Release" && "$(CFG)" != "nt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nt.mak" CFG="nt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nt - Win32 Retail" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "nt - Win32 Retail"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\klif.sys"


CLEAN :
	-@erase "$(INTDIR)\nthook.obj"
	-@erase "$(INTDIR)\nthook.res"
	-@erase "$(INTDIR)\SysIO.obj"
	-@erase "..\Release\klif.sys"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MT /W3 /WX /Oy /Gy /I "$(BASEDIR)\inc" /I "$(CPU)\\" /I "." /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /Fo"$(INTDIR)\\" /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nthook.res" /i "$(BASEDIR)\inc" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nt.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /machine:IX86 /nodefaultlib /out:"..\Release\klif.sys" /libpath:"$(BASEDIR)\lib\i386\free" /driver /debug:NONE /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native 
LINK32_OBJS= \
	"$(INTDIR)\nthook.obj" \
	"$(INTDIR)\SysIO.obj" \
	"$(INTDIR)\nthook.res"

"..\Release\klif.sys" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nt - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\klif.sys"


CLEAN :
	-@erase "$(INTDIR)\nthook.obj"
	-@erase "$(INTDIR)\nthook.res"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\Release\klif.sys"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /Gz /MT /W3 /Zi /Ox /Oi /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\ddk" /I "$(CPU)\\" /I "..\..\..\CommonFiles" /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /Zel -cbstring /QIfdiv- /QIf /GF /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nthook.res" /i "$(BASEDIR)\inc" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nt.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:none /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"..\Release\klif.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(BASEDIR)\libfre\i386" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native /SECTION:.user,ERW,ALIGN=4096 /MERGE:.userd=.user /MERGE:.userc=.user 
LINK32_OBJS= \
	"$(INTDIR)\nthook.obj" \
	"$(INTDIR)\nthook.res"

"..\Release\klif.sys" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=PostBuild step
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\Release\klif.sys"
   PostBuild.cmd Release
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\Debug\klif.sys" "$(OUTDIR)\nt.bsc"


CLEAN :
	-@erase "$(INTDIR)\nthook.obj"
	-@erase "$(INTDIR)\nthook.res"
	-@erase "$(INTDIR)\nthook.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\klif.map"
	-@erase "$(OUTDIR)\nt.bsc"
	-@erase "..\Debug\klif.pdb"
	-@erase "..\Debug\klif.sys"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MTd /W3 /Zi /Oi /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\ddk" /I "$(CPU)\\" /I "..\..\..\CommonFiles" /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_DEBUG" /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /Fr"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Zel -cbstring /QIfdiv- /QIf /GF /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nthook.res" /i "$(BASEDIR)\inc" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nt.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\nthook.sbr"

"$(OUTDIR)\nt.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=xilink6.exe
LINK32_FLAGS=int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /pdb:"..\Debug\klif.pdb" /map:"$(INTDIR)\klif.map" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"..\Debug\klif.sys" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(BASEDIR)\libchk\i386" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native /SECTION:.user,ERW,ALIGN=4096 /MERGE:.userd=.user /MERGE:.userc=.user 
LINK32_OBJS= \
	"$(INTDIR)\nthook.obj" \
	"$(INTDIR)\nthook.res"

"..\Debug\klif.sys" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=extract dbg
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "..\Debug\klif.sys" "$(OUTDIR)\nt.bsc"
   PostBuild.cmd Debug
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
!IF EXISTS("nt.dep")
!INCLUDE "nt.dep"
!ELSE 
!MESSAGE Warning: cannot find "nt.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "nt - Win32 Retail" || "$(CFG)" == "nt - Win32 Release" || "$(CFG)" == "nt - Win32 Debug"
SOURCE=.\diskio.c
SOURCE=.\fastio.c
SOURCE=.\fileio.c
SOURCE=.\firewall.c
SOURCE=.\LLDiskIO.c
SOURCE=.\nthook.c

!IF  "$(CFG)" == "nt - Win32 Retail"


"$(INTDIR)\nthook.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nt - Win32 Release"


"$(INTDIR)\nthook.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nt - Win32 Debug"


"$(INTDIR)\nthook.obj"	"$(INTDIR)\nthook.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\nthook.rc

"$(INTDIR)\nthook.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\registry.c
SOURCE=.\SysIO.c

!IF  "$(CFG)" == "nt - Win32 Retail"


"$(INTDIR)\SysIO.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 


!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on ahfw.dsp
!IF "$(CFG)" == ""
CFG=ahfw - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ahfw - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ahfw - Win32 Release" && "$(CFG)" != "ahfw - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ahfw.mak" CFG="ahfw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ahfw - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ahfw - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ahfw - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/ahfw
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ahfw.ppl"

!ELSE 

ALL : "_KAHUSER - Win32 Release" "$(OUTDIR)\ahfw.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"_KAHUSER - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ahfw.res"
	-@erase "$(INTDIR)\ahfw_imp.obj"
	-@erase "$(INTDIR)\plugin_ahfw.obj"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ahfw.exp"
	-@erase "$(OUTDIR)\ahfw.pdb"
	-@erase "$(OUTDIR)\ahfw.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "../include" /I "../../prague/include" /I "../../prague" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "NDEBUG" /D "AHFW_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "C_STYLE_PROP" /D "DIRECT_SEND_EVENTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\ahfw.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ahfw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib advapi32.lib CKAHUM.lib msvcrt.lib ckahrule.lib /nologo /base:"0x61000000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\ahfw.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\ahfw.ppl" /implib:"$(OUTDIR)\ahfw.lib" /libpath:"../lib/release" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\ahfw_imp.obj" \
	"$(INTDIR)\plugin_ahfw.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\ahfw.res"

"$(OUTDIR)\ahfw.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ahfw.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "_KAHUSER - Win32 Release" "$(OUTDIR)\ahfw.ppl"
   tsigner "\out\Release\ahfw.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ahfw - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/ahfw
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ahfw.ppl"

!ELSE 

ALL : "_KAHUSER - Win32 Debug" "$(OUTDIR)\ahfw.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"_KAHUSER - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ahfw.res"
	-@erase "$(INTDIR)\ahfw_imp.obj"
	-@erase "$(INTDIR)\plugin_ahfw.obj"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ahfw.exp"
	-@erase "$(OUTDIR)\ahfw.ilk"
	-@erase "$(OUTDIR)\ahfw.pdb"
	-@erase "$(OUTDIR)\ahfw.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../prague" /I "../../CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "AHFW_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "C_STYLE_PROP" /D "DIRECT_SEND_EVENTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\ahfw.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ahfw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib advapi32.lib CKAHUM.lib ckahrule.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ahfw.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ahfw.ppl" /implib:"$(OUTDIR)\ahfw.lib" /pdbtype:sept /libpath:"../lib/Debug" /libpath:"../../CommonFiles/DebugDll" /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\ahfw_imp.obj" \
	"$(INTDIR)\plugin_ahfw.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\ahfw.res"

"$(OUTDIR)\ahfw.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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
!IF EXISTS("ahfw.dep")
!INCLUDE "ahfw.dep"
!ELSE 
!MESSAGE Warning: cannot find "ahfw.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ahfw - Win32 Release" || "$(CFG)" == "ahfw - Win32 Debug"
SOURCE=.\ahfw_imp.cpp

"$(INTDIR)\ahfw_imp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_ahfw.cpp

"$(INTDIR)\plugin_ahfw.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\task.cpp

"$(INTDIR)\task.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ahfw.rc

"$(INTDIR)\ahfw.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ahfw - Win32 Release"

"_KAHUSER - Win32 Release" : 
   cd "\Components\Windows\KAH"
   NMAKE /f _KAHUSER.mak BUILD=Release
   cd "..\..\..\PPP\AHFW"

"_KAHUSER - Win32 ReleaseCLEAN" : 
   cd "\Components\Windows\KAH"
   cd "..\..\..\PPP\AHFW"

!ELSEIF  "$(CFG)" == "ahfw - Win32 Debug"

"_KAHUSER - Win32 Debug" : 
   cd "\Components\Windows\KAH"
   NMAKE /f _KAHUSER.mak BUILD=Debug
   cd "..\..\..\PPP\AHFW"

"_KAHUSER - Win32 DebugCLEAN" : 
   cd "\Components\Windows\KAH"
   cd "..\..\..\PPP\AHFW"

!ENDIF 


!ENDIF 


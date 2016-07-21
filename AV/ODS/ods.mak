# Microsoft Developer Studio Generated NMAKE File, Based on ods.dsp
!IF "$(CFG)" == ""
CFG=ods - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ods - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ods - Win32 Release" && "$(CFG)" != "ods - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ods.mak" CFG="ods - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ods - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ods - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ods - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/ods
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ods.ppl"

!ELSE 

ALL : "StartUpEnum2 - Win32 Release" "$(OUTDIR)\ods.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"StartUpEnum2 - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ods.res"
	-@erase "$(INTDIR)\plugin_ods.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\scaners.obj"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ods.exp"
	-@erase "$(OUTDIR)\ods.pdb"
	-@erase "$(OUTDIR)\ods.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "../include" /I "../../prague/include" /I "../../prague" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "NDEBUG" /D "bl_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "C_STYLE_PROP" /D "DIRECT_SEND_EVENTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\ods.res" /i "..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib /nologo /base:"0x66000000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\ods.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\ods.ppl" /implib:"$(OUTDIR)\ods.lib" /libpath:"../lib/release" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_ods.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\scaners.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\ods.res"

"$(OUTDIR)\ods.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ods.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "StartUpEnum2 - Win32 Release" "$(OUTDIR)\ods.ppl"
   call prconvert "\out\Release\ods.ppl"
	tsigner "\out\Release\ods.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ods - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/ods
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ods.ppl"

!ELSE 

ALL : "StartUpEnum2 - Win32 Debug" "$(OUTDIR)\ods.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"StartUpEnum2 - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ods.res"
	-@erase "$(INTDIR)\plugin_ods.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\scaners.obj"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ods.exp"
	-@erase "$(OUTDIR)\ods.ilk"
	-@erase "$(OUTDIR)\ods.pdb"
	-@erase "$(OUTDIR)\ods.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../prague" /I "../../CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "ods_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "C_STYLE_PROP" /D "DIRECT_SEND_EVENTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\ods.res" /i "..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib sign.lib FSSync.lib kernel32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ods.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ods.ppl" /implib:"$(OUTDIR)\ods.lib" /pdbtype:sept /libpath:"../lib/Debug" /libpath:"../../CommonFiles/DebugDll" /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_ods.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\scaners.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\ods.res"

"$(OUTDIR)\ods.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ods.dep")
!INCLUDE "ods.dep"
!ELSE 
!MESSAGE Warning: cannot find "ods.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ods - Win32 Release" || "$(CFG)" == "ods - Win32 Debug"
SOURCE=.\plugin_ods.cpp

"$(INTDIR)\plugin_ods.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\process.cpp

"$(INTDIR)\process.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\scaners.cpp

"$(INTDIR)\scaners.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\task.cpp

"$(INTDIR)\task.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ods.rc

"$(INTDIR)\ods.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ods - Win32 Release"

"StartUpEnum2 - Win32 Release" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Release" 
   cd "..\ODS"

"StartUpEnum2 - Win32 ReleaseCLEAN" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\ODS"

!ELSEIF  "$(CFG)" == "ods - Win32 Debug"

"StartUpEnum2 - Win32 Debug" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Debug" 
   cd "..\ODS"

"StartUpEnum2 - Win32 DebugCLEAN" : 
   cd "\PPP\StartUpEnum2"
   $(MAKE) /$(MAKEFLAGS) /F .\startupenum2.mak CFG="StartUpEnum2 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\ODS"

!ENDIF 


!ENDIF 


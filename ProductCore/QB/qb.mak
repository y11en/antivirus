# Microsoft Developer Studio Generated NMAKE File, Based on qb.dsp
!IF "$(CFG)" == ""
CFG=qb - Win32 Debug
!MESSAGE No configuration specified. Defaulting to qb - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "qb - Win32 Release" && "$(CFG)" != "qb - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qb.mak" CFG="qb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "qb - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/qb
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\qb.ppl"

!ELSE 

ALL : "pr_remote - Win32 Release" "$(OUTDIR)\qb.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"pr_remote - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\plugin_qb.obj"
	-@erase "$(INTDIR)\posio_sp.obj"
	-@erase "$(INTDIR)\qb.obj"
	-@erase "$(INTDIR)\qb.res"
	-@erase "$(INTDIR)\qbrestorer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\qb.exp"
	-@erase "$(OUTDIR)\qb.pdb"
	-@erase "$(OUTDIR)\qb.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "." /I "../include" /I "../../prague/include" /I "../../prague" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ods_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\qb.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\qb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libc.lib /nologo /base:"0x66a00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\qb.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\qb.ppl" /implib:"$(OUTDIR)\qb.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_qb.obj" \
	"$(INTDIR)\posio_sp.obj" \
	"$(INTDIR)\qb.obj" \
	"$(INTDIR)\qbrestorer.obj" \
	"$(INTDIR)\qb.res" \
	"$(OUTDIR)\pr_remote.lib"

"$(OUTDIR)\qb.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\qb.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "pr_remote - Win32 Release" "$(OUTDIR)\qb.ppl"
   call prconvert "\out\Release\qb.ppl"
	tsigner "\out\Release\qb.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "qb - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/qb
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\qb.ppl"

!ELSE 

ALL : "pr_remote - Win32 Debug" "$(OUTDIR)\qb.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"pr_remote - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\plugin_qb.obj"
	-@erase "$(INTDIR)\posio_sp.obj"
	-@erase "$(INTDIR)\qb.obj"
	-@erase "$(INTDIR)\qb.res"
	-@erase "$(INTDIR)\qbrestorer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\qb.exp"
	-@erase "$(OUTDIR)\qb.ilk"
	-@erase "$(OUTDIR)\qb.pdb"
	-@erase "$(OUTDIR)\qb.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../prague" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\qb.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\qb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /dll /incremental:yes /pdb:"$(OUTDIR)\qb.pdb" /debug /machine:I386 /out:"$(OUTDIR)\qb.ppl" /implib:"$(OUTDIR)\qb.lib" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_qb.obj" \
	"$(INTDIR)\posio_sp.obj" \
	"$(INTDIR)\qb.obj" \
	"$(INTDIR)\qbrestorer.obj" \
	"$(INTDIR)\qb.res" \
	"$(OUTDIR)\pr_remote.lib"

"$(OUTDIR)\qb.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("qb.dep")
!INCLUDE "qb.dep"
!ELSE 
!MESSAGE Warning: cannot find "qb.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "qb - Win32 Release" || "$(CFG)" == "qb - Win32 Debug"
SOURCE=.\plugin_qb.cpp

"$(INTDIR)\plugin_qb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\posio_sp.cpp

"$(INTDIR)\posio_sp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\qb.cpp

"$(INTDIR)\qb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\qbrestorer.cpp

"$(INTDIR)\qbrestorer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\qb.rc

"$(INTDIR)\qb.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "qb - Win32 Release"

"pr_remote - Win32 Release" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Release" 
   cd "..\..\PPP\QB"

"pr_remote - Win32 ReleaseCLEAN" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\QB"

!ELSEIF  "$(CFG)" == "qb - Win32 Debug"

"pr_remote - Win32 Debug" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Debug" 
   cd "..\..\PPP\QB"

"pr_remote - Win32 DebugCLEAN" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\QB"

!ENDIF 


!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on btimages.dsp
!IF "$(CFG)" == ""
CFG=btimages - Win32 Debug
!MESSAGE No configuration specified. Defaulting to btimages - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "btimages - Win32 Release" && "$(CFG)" != "btimages - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "btimages.mak" CFG="btimages - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "btimages - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "btimages - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "btimages - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/btimages
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\btimages.ppl"


CLEAN :
	-@erase "$(INTDIR)\bti_imp.obj"
	-@erase "$(INTDIR)\btimages.obj"
	-@erase "$(INTDIR)\plugin_btimages.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\btimages.exp"
	-@erase "$(OUTDIR)\btimages.pdb"
	-@erase "$(OUTDIR)\btimages.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "btimages_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\btimages.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x62a00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\btimages.pdb" /debug /machine:I386 /out:"$(OUTDIR)\btimages.ppl" /implib:"$(OUTDIR)\btimages.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDLL" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\bti_imp.obj" \
	"$(INTDIR)\btimages.obj" \
	"$(INTDIR)\plugin_btimages.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\btimages.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\btimages.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\btimages.ppl"
   tsigner "\out\Release\btimages.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "btimages - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/btimages
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\btimages.ppl"


CLEAN :
	-@erase "$(INTDIR)\bti_imp.obj"
	-@erase "$(INTDIR)\btimages.obj"
	-@erase "$(INTDIR)\plugin_btimages.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\btimages.exp"
	-@erase "$(OUTDIR)\btimages.ilk"
	-@erase "$(OUTDIR)\btimages.pdb"
	-@erase "$(OUTDIR)\btimages.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "btimages_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\btimages.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /dll /incremental:yes /pdb:"$(OUTDIR)\btimages.pdb" /debug /machine:I386 /out:"$(OUTDIR)\btimages.ppl" /implib:"$(OUTDIR)\btimages.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDLL" 
LINK32_OBJS= \
	"$(INTDIR)\bti_imp.obj" \
	"$(INTDIR)\btimages.obj" \
	"$(INTDIR)\plugin_btimages.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\btimages.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("btimages.dep")
!INCLUDE "btimages.dep"
!ELSE 
!MESSAGE Warning: cannot find "btimages.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "btimages - Win32 Release" || "$(CFG)" == "btimages - Win32 Debug"
SOURCE=.\bti_imp.cpp

"$(INTDIR)\bti_imp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\btimages.cpp

"$(INTDIR)\btimages.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_btimages.cpp

"$(INTDIR)\plugin_btimages.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\template.rc

"$(INTDIR)\template.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on Unreduce.dsp
!IF "$(CFG)" == ""
CFG=Unreduce - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Unreduce - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Unreduce - Win32 Release" && "$(CFG)" != "Unreduce - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Unreduce.mak" CFG="Unreduce - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Unreduce - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Unreduce - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Unreduce - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Extract/Unreduce
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\Unreduce.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_unreduce.obj"
	-@erase "$(INTDIR)\unreduce.res"
	-@erase "$(INTDIR)\UNREDUCE_.OBJ"
	-@erase "$(INTDIR)\unreduce_t.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Unreduce.exp"
	-@erase "$(OUTDIR)\Unreduce.pdb"
	-@erase "$(OUTDIR)\Unreduce.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNREDUCE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\unreduce.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Unreduce.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x68800000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\Unreduce.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Unreduce.ppl" /implib:"$(OUTDIR)\Unreduce.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_unreduce.obj" \
	"$(INTDIR)\UNREDUCE_.OBJ" \
	"$(INTDIR)\unreduce_t.obj" \
	"$(INTDIR)\unreduce.res"

"$(OUTDIR)\Unreduce.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\Unreduce.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Unreduce.ppl"
   call prconvert "\out\Release\Unreduce.ppl"
	tsigner "\out\Release\Unreduce.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Unreduce - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Extract/Unreduce
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\Unreduce.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_unreduce.obj"
	-@erase "$(INTDIR)\unreduce.res"
	-@erase "$(INTDIR)\UNREDUCE_.OBJ"
	-@erase "$(INTDIR)\unreduce_t.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Unreduce.exp"
	-@erase "$(OUTDIR)\Unreduce.ilk"
	-@erase "$(OUTDIR)\Unreduce.pdb"
	-@erase "$(OUTDIR)\Unreduce.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNREDUCE_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\unreduce.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Unreduce.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Unreduce.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Unreduce.ppl" /implib:"$(OUTDIR)\Unreduce.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_unreduce.obj" \
	"$(INTDIR)\UNREDUCE_.OBJ" \
	"$(INTDIR)\unreduce_t.obj" \
	"$(INTDIR)\unreduce.res"

"$(OUTDIR)\Unreduce.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Unreduce.dep")
!INCLUDE "Unreduce.dep"
!ELSE 
!MESSAGE Warning: cannot find "Unreduce.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Unreduce - Win32 Release" || "$(CFG)" == "Unreduce - Win32 Debug"
SOURCE=.\plugin_unreduce.c

"$(INTDIR)\plugin_unreduce.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UNREDUCE_.C

"$(INTDIR)\UNREDUCE_.OBJ" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unreduce_t.c

"$(INTDIR)\unreduce_t.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unreduce.rc

"$(INTDIR)\unreduce.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


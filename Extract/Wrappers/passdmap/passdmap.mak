# Microsoft Developer Studio Generated NMAKE File, Based on passdmap.dsp
!IF "$(CFG)" == ""
CFG=passdmap - Win32 Debug
!MESSAGE No configuration specified. Defaulting to passdmap - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "passdmap - Win32 Release" && "$(CFG)" != "passdmap - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "passdmap.mak" CFG="passdmap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "passdmap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "passdmap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "passdmap - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Wrappers/passdmap
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\passdmap.ppl"


CLEAN :
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\dmap.obj"
	-@erase "$(INTDIR)\dmap.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\passdmap.exp"
	-@erase "$(OUTDIR)\passdmap.pdb"
	-@erase "$(OUTDIR)\passdmap.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PASSDMAP_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dmap.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\passdmap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x66300000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\passdmap.pdb" /debug /machine:I386 /out:"$(OUTDIR)\passdmap.ppl" /implib:"$(OUTDIR)\passdmap.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\dmap.obj" \
	"$(INTDIR)\dmap.res"

"$(OUTDIR)\passdmap.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\passdmap.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\passdmap.ppl"
   call prconvert "\out\Release\passdmap.ppl"
	tsigner "\out\Release\passdmap.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "passdmap - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Wrappers/passdmap
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\passdmap.ppl"


CLEAN :
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\dmap.obj"
	-@erase "$(INTDIR)\dmap.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\passdmap.exp"
	-@erase "$(OUTDIR)\passdmap.ilk"
	-@erase "$(OUTDIR)\passdmap.pdb"
	-@erase "$(OUTDIR)\passdmap.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PASSDMAP_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dmap.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\passdmap.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\passdmap.pdb" /debug /machine:I386 /out:"$(OUTDIR)\passdmap.ppl" /implib:"$(OUTDIR)\passdmap.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\dmap.obj" \
	"$(INTDIR)\dmap.res"

"$(OUTDIR)\passdmap.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("passdmap.dep")
!INCLUDE "passdmap.dep"
!ELSE 
!MESSAGE Warning: cannot find "passdmap.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "passdmap - Win32 Release" || "$(CFG)" == "passdmap - Win32 Debug"
SOURCE=.\crypt.c

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dmap.c

"$(INTDIR)\dmap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dmap.rc

"$(INTDIR)\dmap.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


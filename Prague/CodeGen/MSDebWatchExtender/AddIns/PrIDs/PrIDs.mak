# Microsoft Developer Studio Generated NMAKE File, Based on PrIDs.dsp
!IF "$(CFG)" == ""
CFG=PrIDs - Win32 Debug
!MESSAGE No configuration specified. Defaulting to PrIDs - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PrIDs - Win32 Release" && "$(CFG)" != "PrIDs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PrIDs.mak" CFG="PrIDs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PrIDs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PrIDs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "PrIDs - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\release\PrIDs.dll"


CLEAN :
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\PrIDs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PrIDs.exp"
	-@erase "$(OUTDIR)\PrIDs.lib"
	-@erase "$(OUTDIR)\PrIDs.pdb"
	-@erase "..\..\release\PrIDs.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /I "../../../../include" /I "../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PrIDs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PrIDs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\PrIDs.pdb" /debug /machine:I386 /def:".\PrIDs.def" /out:"../../release/PrIDs.dll" /implib:"$(OUTDIR)\PrIDs.lib" 
DEF_FILE= \
	".\PrIDs.def"
LINK32_OBJS= \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\PrIDs.obj"

"..\..\release\PrIDs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PrIDs - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "e:\ds\Common\MSDev98\Bin\PrIDs.dll"


CLEAN :
	-@erase "$(INTDIR)\fcache.obj"
	-@erase "$(INTDIR)\PrIDs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PrIDs.exp"
	-@erase "$(OUTDIR)\PrIDs.lib"
	-@erase "$(OUTDIR)\PrIDs.pdb"
	-@erase "e:\ds\Common\MSDev98\Bin\PrIDs.dll"
	-@erase "e:\ds\Common\MSDev98\Bin\PrIDs.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "../.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\PrIDs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PrIDs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\PrIDs.pdb" /debug /machine:I386 /def:".\PrIDs.def" /out:"e:\ds\Common\MSDev98\Bin\PrIDs.dll" /implib:"$(OUTDIR)\PrIDs.lib" /pdbtype:sept 
DEF_FILE= \
	".\PrIDs.def"
LINK32_OBJS= \
	"$(INTDIR)\fcache.obj" \
	"$(INTDIR)\PrIDs.obj"

"e:\ds\Common\MSDev98\Bin\PrIDs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("PrIDs.dep")
!INCLUDE "PrIDs.dep"
!ELSE 
!MESSAGE Warning: cannot find "PrIDs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "PrIDs - Win32 Release" || "$(CFG)" == "PrIDs - Win32 Debug"
SOURCE=..\..\Common\fcache.c

"$(INTDIR)\fcache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\PrIDs.c

"$(INTDIR)\PrIDs.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


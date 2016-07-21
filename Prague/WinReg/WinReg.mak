# Microsoft Developer Studio Generated NMAKE File, Based on WinReg.dsp
!IF "$(CFG)" == ""
CFG=WinReg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WinReg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WinReg - Win32 Release" && "$(CFG)" != "WinReg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinReg.mak" CFG="WinReg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinReg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinReg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "WinReg - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/WinReg
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\WinReg.ppl"


CLEAN :
	-@erase "$(INTDIR)\dt_copy.obj"
	-@erase "$(INTDIR)\plugin_win32_reg.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinReg.exp"
	-@erase "$(OUTDIR)\WinReg.pdb"
	-@erase "$(OUTDIR)\WinReg.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WINREG_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\registry.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinReg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x69500000" /dll /incremental:no /pdb:"$(OUTDIR)\WinReg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WinReg.ppl" /implib:"$(OUTDIR)\WinReg.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\dt_copy.obj" \
	"$(INTDIR)\plugin_win32_reg.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\registry.res"

"$(OUTDIR)\WinReg.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\WinReg.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\WinReg.ppl"
   tsigner "\out\Release\WinReg.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "WinReg - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/WinReg
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\WinReg.ppl"


CLEAN :
	-@erase "$(INTDIR)\dt_copy.obj"
	-@erase "$(INTDIR)\plugin_win32_reg.obj"
	-@erase "$(INTDIR)\registry.obj"
	-@erase "$(INTDIR)\registry.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinReg.exp"
	-@erase "$(OUTDIR)\WinReg.ilk"
	-@erase "$(OUTDIR)\WinReg.pdb"
	-@erase "$(OUTDIR)\WinReg.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WINREG_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\registry.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinReg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\WinReg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WinReg.ppl" /implib:"$(OUTDIR)\WinReg.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dt_copy.obj" \
	"$(INTDIR)\plugin_win32_reg.obj" \
	"$(INTDIR)\registry.obj" \
	"$(INTDIR)\registry.res"

"$(OUTDIR)\WinReg.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("WinReg.dep")
!INCLUDE "WinReg.dep"
!ELSE 
!MESSAGE Warning: cannot find "WinReg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinReg - Win32 Release" || "$(CFG)" == "WinReg - Win32 Debug"
SOURCE=..\DTReg\dt_copy.cpp

"$(INTDIR)\dt_copy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_win32_reg.c

"$(INTDIR)\plugin_win32_reg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\registry.c

!IF  "$(CFG)" == "WinReg - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WINREG_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinReg - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WINREG_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\registry.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\registry.rc

"$(INTDIR)\registry.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


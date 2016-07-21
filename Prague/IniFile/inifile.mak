# Microsoft Developer Studio Generated NMAKE File, Based on inifile.dsp
!IF "$(CFG)" == ""
CFG=IniFile - Win32 Debug
!MESSAGE No configuration specified. Defaulting to IniFile - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "IniFile - Win32 Release" && "$(CFG)" != "IniFile - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "inifile.mak" CFG="IniFile - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IniFile - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IniFile - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "IniFile - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\inifile.ppl"


CLEAN :
	-@erase "$(INTDIR)\inifile.obj"
	-@erase "$(INTDIR)\inifile_res.res"
	-@erase "$(INTDIR)\IniLib.obj"
	-@erase "$(INTDIR)\plugin_inifile.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\inifile.exp"
	-@erase "$(OUTDIR)\inifile.pdb"
	-@erase "$(OUTDIR)\inifile.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\prague\include" /I "..\..\commonfiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\inifile_res.res" /i "..\..\commonfiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\inifile.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x64900000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\inifile.pdb" /debug /machine:I386 /out:"$(OUTDIR)\inifile.ppl" /implib:"$(OUTDIR)\inifile.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\inifile.obj" \
	"$(INTDIR)\IniLib.obj" \
	"$(INTDIR)\plugin_inifile.obj" \
	"$(INTDIR)\inifile_res.res"

"$(OUTDIR)\inifile.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\inifile.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\inifile.ppl"
   call prconvert "\out\Release\inifile.ppl"
	tsigner "\out\Release\inifile.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "IniFile - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\inifile.ppl"


CLEAN :
	-@erase "$(INTDIR)\inifile.obj"
	-@erase "$(INTDIR)\inifile_res.res"
	-@erase "$(INTDIR)\IniLib.obj"
	-@erase "$(INTDIR)\plugin_inifile.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\inifile.exp"
	-@erase "$(OUTDIR)\inifile.ilk"
	-@erase "$(OUTDIR)\inifile.pdb"
	-@erase "$(OUTDIR)\inifile.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\prague\include" /I "..\..\commonfiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\inifile_res.res" /i "..\..\commonfiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\inifile.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\inifile.pdb" /debug /machine:I386 /out:"$(OUTDIR)\inifile.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\inifile.obj" \
	"$(INTDIR)\IniLib.obj" \
	"$(INTDIR)\plugin_inifile.obj" \
	"$(INTDIR)\inifile_res.res"

"$(OUTDIR)\inifile.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("inifile.dep")
!INCLUDE "inifile.dep"
!ELSE 
!MESSAGE Warning: cannot find "inifile.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "IniFile - Win32 Release" || "$(CFG)" == "IniFile - Win32 Debug"
SOURCE=.\inifile.cpp

"$(INTDIR)\inifile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\Prague\IniLib\IniLib.c

"$(INTDIR)\IniLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_inifile.cpp

"$(INTDIR)\plugin_inifile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\inifile_res.rc"

"$(INTDIR)\inifile_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


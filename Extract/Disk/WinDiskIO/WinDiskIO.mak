# Microsoft Developer Studio Generated NMAKE File, Based on WinDiskIO.dsp
!IF "$(CFG)" == ""
CFG=WinDiskIO - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WinDiskIO - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WinDiskIO - Win32 Release" && "$(CFG)" != "WinDiskIO - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinDiskIO.mak" CFG="WinDiskIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinDiskIO - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WinDiskIO - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "WinDiskIO - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/WinDiskIO
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\WDiskIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\EvThread.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\plugin_windiskio.obj"
	-@erase "$(INTDIR)\ptrthread.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WDiskIO.exp"
	-@erase "$(OUTDIR)\WDiskIO.pdb"
	-@erase "$(OUTDIR)\WDiskIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WINDISKIO_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinDiskIO.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib sign.lib /nologo /base:"0x69300000" /dll /incremental:no /pdb:"$(OUTDIR)\WDiskIO.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WDiskIO.ppl" /implib:"$(OUTDIR)\WDiskIO.lib" /pdbtype:sept /libpath:"..\..\out\Release" /libpath:"..\..\CommonFiles\Release\\" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\EvThread.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\plugin_windiskio.obj" \
	"$(INTDIR)\ptrthread.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\WDiskIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\WDiskIO.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\WDiskIO.ppl"
   tsigner "\out\Release\WDiskIO.ppl
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "WinDiskIO - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/WinDiskIO
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\WDiskIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\EvThread.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\plugin_windiskio.obj"
	-@erase "$(INTDIR)\ptrthread.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WDiskIO.exp"
	-@erase "$(OUTDIR)\WDiskIO.ilk"
	-@erase "$(OUTDIR)\WDiskIO.pdb"
	-@erase "$(OUTDIR)\WDiskIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WINDISKIO_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinDiskIO.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib sign.lib /nologo /base:"0xADC00000" /dll /incremental:yes /pdb:"$(OUTDIR)\WDiskIO.pdb" /debug /machine:I386 /out:"$(OUTDIR)\WDiskIO.ppl" /implib:"$(OUTDIR)\WDiskIO.lib" /pdbtype:sept /libpath:"..\..\out\Debug" /libpath:"..\..\CommonFiles\DebugDll\\" 
LINK32_OBJS= \
	"$(INTDIR)\EvThread.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\plugin_windiskio.obj" \
	"$(INTDIR)\ptrthread.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\WDiskIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("WinDiskIO.dep")
!INCLUDE "WinDiskIO.dep"
!ELSE 
!MESSAGE Warning: cannot find "WinDiskIO.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinDiskIO - Win32 Release" || "$(CFG)" == "WinDiskIO - Win32 Debug"
SOURCE=.\EvThread.cpp

"$(INTDIR)\EvThread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objptr.c

"$(INTDIR)\objptr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_windiskio.c

"$(INTDIR)\plugin_windiskio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ptrthread.cpp

"$(INTDIR)\ptrthread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\template.rc

"$(INTDIR)\template.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


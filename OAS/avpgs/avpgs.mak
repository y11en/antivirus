# Microsoft Developer Studio Generated NMAKE File, Based on avpgs.dsp
!IF "$(CFG)" == ""
CFG=avpg - Win32 Debug WithoutStopping
!MESSAGE No configuration specified. Defaulting to avpg - Win32 Debug WithoutStopping.
!ENDIF 

!IF "$(CFG)" != "avpg - Win32 Release" && "$(CFG)" != "avpg - Win32 Debug" && "$(CFG)" != "avpg - Win32 Debug WithoutStopping"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avpgs.mak" CFG="avpg - Win32 Debug WithoutStopping"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avpg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avpg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avpg - Win32 Debug WithoutStopping" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avpg - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/avpgs
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\avpgs.ppl"


CLEAN :
	-@erase "$(INTDIR)\avpg.obj"
	-@erase "$(INTDIR)\avpg.res"
	-@erase "$(INTDIR)\avpgimpl.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\EvCache.obj"
	-@erase "$(INTDIR)\exphooks.obj"
	-@erase "$(INTDIR)\hookbase.obj"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\plugin_avpg.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avpgs.exp"
	-@erase "$(OUTDIR)\avpgs.pdb"
	-@erase "$(OUTDIR)\avpgs.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\avpg.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avpgs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib sign.lib /nologo /base:"0x61f00000" /dll /incremental:no /pdb:"$(OUTDIR)\avpgs.pdb" /debug /machine:I386 /def:".\avpgs.def" /out:"$(OUTDIR)\avpgs.ppl" /implib:"$(OUTDIR)\avpgs.lib" /pdbtype:sept /libpath:"..\..\out\Release" /libpath:"..\..\CommonFiles\ReleaseDll\\" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\avpg.obj" \
	"$(INTDIR)\avpgimpl.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\EvCache.obj" \
	"$(INTDIR)\exphooks.obj" \
	"$(INTDIR)\hookbase.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\plugin_avpg.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\avpg.res"

"$(OUTDIR)\avpgs.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\avpgs.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\avpgs.ppl"
   tsigner "\out\Release\avpgs.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/avpgs
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\avpgs.ppl"


CLEAN :
	-@erase "$(INTDIR)\avpg.obj"
	-@erase "$(INTDIR)\avpg.res"
	-@erase "$(INTDIR)\avpgimpl.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\EvCache.obj"
	-@erase "$(INTDIR)\exphooks.obj"
	-@erase "$(INTDIR)\hookbase.obj"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\plugin_avpg.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avpgs.exp"
	-@erase "$(OUTDIR)\avpgs.ilk"
	-@erase "$(OUTDIR)\avpgs.pdb"
	-@erase "$(OUTDIR)\avpgs.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\avpg.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avpgs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib sign.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\avpgs.pdb" /debug /machine:I386 /def:".\avpgs.def" /out:"$(OUTDIR)\avpgs.ppl" /implib:"$(OUTDIR)\avpgs.lib" /pdbtype:sept /libpath:"..\..\out\Debug" /libpath:"..\..\CommonFiles\DebugDll\\" /IGNORE:6004 
DEF_FILE= \
	".\avpgs.def"
LINK32_OBJS= \
	"$(INTDIR)\avpg.obj" \
	"$(INTDIR)\avpgimpl.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\EvCache.obj" \
	"$(INTDIR)\exphooks.obj" \
	"$(INTDIR)\hookbase.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\plugin_avpg.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\avpg.res"

"$(OUTDIR)\avpgs.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug WithoutStopping"

OUTDIR=.\../../out/Debug WithoutStopping
INTDIR=.\../../temp/Debug WithoutStopping/prague/avpgs

ALL : "..\..\out\Debug\avpgs.ppl"


CLEAN :
	-@erase "$(INTDIR)\avpg.obj"
	-@erase "$(INTDIR)\avpg.res"
	-@erase "$(INTDIR)\avpgimpl.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\EvCache.obj"
	-@erase "$(INTDIR)\exphooks.obj"
	-@erase "$(INTDIR)\hookbase.obj"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\plugin_avpg.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avpgs.exp"
	-@erase "$(OUTDIR)\avpgs.pdb"
	-@erase "..\..\out\Debug\avpgs.ilk"
	-@erase "..\..\out\Debug\avpgs.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /Zi /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /D "_WITHOUTSTOPPING_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\avpg.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avpgs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib sign.lib /nologo /base:"0xAD100000" /dll /incremental:yes /pdb:"$(OUTDIR)\avpgs.pdb" /debug /machine:I386 /def:".\avpgs.def" /out:"../../out/Debug/avpgs.ppl" /implib:"$(OUTDIR)\avpgs.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll\\" 
DEF_FILE= \
	".\avpgs.def"
LINK32_OBJS= \
	"$(INTDIR)\avpg.obj" \
	"$(INTDIR)\avpgimpl.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\EvCache.obj" \
	"$(INTDIR)\exphooks.obj" \
	"$(INTDIR)\hookbase.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\plugin_avpg.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\avpg.res"

"..\..\out\Debug\avpgs.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Debug\avpgs.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Sign "avpg" plugin
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\out\Debug\avpgs.ppl"
   tsigner "\out\Debug\avpgs.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

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
!IF EXISTS("avpgs.dep")
!INCLUDE "avpgs.dep"
!ELSE 
!MESSAGE Warning: cannot find "avpgs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avpg - Win32 Release" || "$(CFG)" == "avpg - Win32 Debug" || "$(CFG)" == "avpg - Win32 Debug WithoutStopping"
SOURCE=.\avpg.cpp

"$(INTDIR)\avpg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avpgimpl.cpp

"$(INTDIR)\avpgimpl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\debug.cpp

"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DrvEventList.cpp

"$(INTDIR)\DrvEventList.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EvCache.cpp

"$(INTDIR)\EvCache.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\exphooks.cpp

"$(INTDIR)\exphooks.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\Windows\Hook\R3Hook\hookbase.c

"$(INTDIR)\hookbase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\job_thread.cpp

"$(INTDIR)\job_thread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OwnSync.cpp

"$(INTDIR)\OwnSync.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_avpg.cpp

"$(INTDIR)\plugin_avpg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TaskThread.cpp

"$(INTDIR)\TaskThread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avpg.rc

"$(INTDIR)\avpg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on FSSync.dsp
!IF "$(CFG)" == ""
CFG=FSSync - Win32 DebugS AutoThreadReg
!MESSAGE No configuration specified. Defaulting to FSSync - Win32 DebugS AutoThreadReg.
!ENDIF 

!IF "$(CFG)" != "FSSync - Win32 Release" && "$(CFG)" != "FSSync - Win32 Debug" && "$(CFG)" != "FSSync - Win32 DebugS" && "$(CFG)" != "FSSync - Win32 ReleaseS" && "$(CFG)" != "FSSync - Win32 DebugS AutoThreadReg"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FSSync.mak" CFG="FSSync - Win32 DebugS AutoThreadReg"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSSync - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 DebugS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 ReleaseS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 DebugS AutoThreadReg" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "FSSync - Win32 Release"

OUTDIR=.\Release
INTDIR=.\..\..\..\temp\Release\fsdrv

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\out\Release\FSSync.dll" "..\..\..\temp\Release\fsdrv\FSSync.pch"

!ELSE 

ALL : "FSDrvLib - Win32 Release" "..\..\..\out\Release\FSSync.dll" "..\..\..\temp\Release\fsdrv\FSSync.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FSDrvLib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\FSSync.obj"
	-@erase "$(INTDIR)\FSSync.pch"
	-@erase "$(INTDIR)\FSSync.res"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\proactive.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\out\Release\FSSync.dll"
	-@erase "..\..\..\out\Release\FSSync.exp"
	-@erase "..\..\..\out\Release\FSSync.lib"
	-@erase "..\..\..\out\Release\FSSync.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\Hook" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\FSSync.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSSync.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /base:"0x63b00000" /dll /incremental:no /pdb:"..\..\..\out\Release/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\Release/FSSync.dll" /implib:"..\..\..\out\Release/FSSync.lib" /libpath:"..\..\..\out\Release" /libpath:"..\..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\FSSync.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\proactive.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\FSSync.res" \
	"..\..\..\out\Release\FSDrvLib.lib"

"..\..\..\out\Release\FSSync.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\FSSync.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "FSDrvLib - Win32 Release" "..\..\..\out\Release\FSSync.dll" "..\..\..\temp\Release\fsdrv\FSSync.pch"
   call tsigner "\out\Release\FSSync.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\..\..\..\temp\Debug\fsdrv

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\out\Debug\FSSync.dll" "..\..\..\temp\Debug\fsdrv\FSSync.pch"

!ELSE 

ALL : "FSDrvLib - Win32 Debug" "..\..\..\out\Debug\FSSync.dll" "..\..\..\temp\Debug\fsdrv\FSSync.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FSDrvLib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\FSSync.obj"
	-@erase "$(INTDIR)\FSSync.pch"
	-@erase "$(INTDIR)\FSSync.res"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\proactive.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\out\Debug\FSSync.dll"
	-@erase "..\..\..\out\Debug\FSSync.exp"
	-@erase "..\..\..\out\Debug\FSSync.ilk"
	-@erase "..\..\..\out\Debug\FSSync.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\Hook" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\FSSync.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSSync.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=FSDrvLib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"..\..\..\out\Debug/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\Debug/FSSync.dll" /implib:"..\..\..\out\Debug/FSSync.lib" /pdbtype:sept /libpath:"..\..\..\out\Debug" /libpath:"..\..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\FSSync.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\proactive.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\FSSync.res" \
	"..\..\..\out\Debug\FSDrvLib.lib"

"..\..\..\out\Debug\FSSync.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Debug\FSSync.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "FSDrvLib - Win32 Debug" "..\..\..\out\Debug\FSSync.dll" "..\..\..\temp\Debug\fsdrv\FSSync.pch"
   call tsigner "\out\Debug\FSSync.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

OUTDIR=.\DebugS
INTDIR=.\..\..\..\temp\DebugS\fsdrv

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\out\DebugS\FSSync.dll"

!ELSE 

ALL : "FSDrvLib - Win32 DebugS" "..\..\..\out\DebugS\FSSync.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FSDrvLib - Win32 DebugSCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\FSSync.obj"
	-@erase "$(INTDIR)\FSSync.pch"
	-@erase "$(INTDIR)\FSSync.res"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\proactive.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\out\DebugS\FSSync.dll"
	-@erase "..\..\..\out\DebugS\FSSync.exp"
	-@erase "..\..\..\out\DebugS\FSSync.ilk"
	-@erase "..\..\..\out\DebugS\FSSync.lib"
	-@erase "..\..\..\out\DebugS\FSSync.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fp"$(INTDIR)\FSSync.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\FSSync.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSSync.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=sign.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /incremental:yes /pdb:"..\..\..\out\DebugS/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\DebugS/FSSync.dll" /implib:"..\..\..\out\DebugS\FSSync.lib" /pdbtype:sept /libpath:"..\..\..\out\DebugS" /libpath:"..\..\..\CommonFiles\Debug" 
LINK32_OBJS= \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\FSSync.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\proactive.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\FSSync.res" \
	"..\..\..\out\DebugS\FSDrvLib.lib"

"..\..\..\out\DebugS\FSSync.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\DebugS\FSSync.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "FSDrvLib - Win32 DebugS" "..\..\..\out\DebugS\FSSync.dll"
   call tsigner "\out\DebugS\FSSync.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

OUTDIR=.\ReleaseS
INTDIR=.\..\..\..\temp\ReleaseS\fsdrv

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\out\ReleaseS\FSSync.dll"

!ELSE 

ALL : "FSDrvLib - Win32 ReleaseS" "..\..\..\out\ReleaseS\FSSync.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FSDrvLib - Win32 ReleaseSCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\FSSync.obj"
	-@erase "$(INTDIR)\FSSync.pch"
	-@erase "$(INTDIR)\FSSync.res"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\proactive.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\out\ReleaseS\FSSync.dll"
	-@erase "..\..\..\out\ReleaseS\FSSync.exp"
	-@erase "..\..\..\out\ReleaseS\FSSync.lib"
	-@erase "..\..\..\out\ReleaseS\FSSync.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MT /W3 /GX /Zd /O2 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fp"$(INTDIR)\FSSync.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\FSSync.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSSync.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib sign.lib /nologo /dll /incremental:no /pdb:"..\..\..\out\ReleaseS/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\ReleaseS\FSSync.dll" /implib:"..\..\..\out\ReleaseS/FSSync.lib" /libpath:"..\..\..\out\ReleaseS" /libpath:"..\..\..\CommonFiles\Release" 
LINK32_OBJS= \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\FSSync.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\proactive.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\FSSync.res" \
	"..\..\..\out\ReleaseS\FSDrvLib.lib"

"..\..\..\out\ReleaseS\FSSync.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\ReleaseS\FSSync.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "FSDrvLib - Win32 ReleaseS" "..\..\..\out\ReleaseS\FSSync.dll"
   call tsigner "\out\ReleaseS\FSSync.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

OUTDIR=.\FSSync___Win32_DebugDll_AutoThreadReg
INTDIR=.\FSSync___Win32_DebugDll_AutoThreadReg

!IF "$(RECURSE)" == "0" 

ALL : "..\..\..\CommonFiles\DebugDll\FSSync.dll"

!ELSE 

ALL : "..\..\..\CommonFiles\DebugDll\FSSync.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\DrvEventList.obj"
	-@erase "$(INTDIR)\FSSync.obj"
	-@erase "$(INTDIR)\FSSync.pch"
	-@erase "$(INTDIR)\FSSync.res"
	-@erase "$(INTDIR)\job_thread.obj"
	-@erase "$(INTDIR)\OwnSync.obj"
	-@erase "$(INTDIR)\proactive.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TaskThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\CommonFiles\DebugDll\FSSync.dll"
	-@erase "..\..\..\CommonFiles\DebugDll\FSSync.exp"
	-@erase "..\..\..\CommonFiles\DebugDll\FSSync.ilk"
	-@erase "..\..\..\CommonFiles\DebugDll\FSSync.lib"
	-@erase "..\..\..\CommonFiles\DebugDll\FSSync.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\CommonFiles" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /D "FSDRV_AUTOTHREADREGISTER" /Fp"$(INTDIR)\FSSync.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\FSSync.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSSync.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=sign.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /incremental:yes /pdb:"..\..\..\CommonFiles\DebugDll/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\CommonFiles\DebugDll/FSSync.dll" /implib:"..\..\..\CommonFiles\DebugDll\FSSync.lib" /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDll" 
DEF_FILE= \
	".\FSSync.def"
LINK32_OBJS= \
	"$(INTDIR)\DrvEventList.obj" \
	"$(INTDIR)\FSSync.obj" \
	"$(INTDIR)\job_thread.obj" \
	"$(INTDIR)\OwnSync.obj" \
	"$(INTDIR)\proactive.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TaskThread.obj" \
	"$(INTDIR)\FSSync.res"

"..\..\..\CommonFiles\DebugDll\FSSync.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\CommonFiles\DebugDll\FSSync.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\CommonFiles\DebugDll\FSSync.dll"
   tsigner "\CommonFiles\DebugDll\FSSync.dll"
	PostBuild DebugDll
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
!IF EXISTS("FSSync.dep")
!INCLUDE "FSSync.dep"
!ELSE 
!MESSAGE Warning: cannot find "FSSync.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FSSync - Win32 Release" || "$(CFG)" == "FSSync - Win32 Debug" || "$(CFG)" == "FSSync - Win32 DebugS" || "$(CFG)" == "FSSync - Win32 ReleaseS" || "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"
SOURCE=.\proactive\DrvEventList.cpp

"$(INTDIR)\DrvEventList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\FSSync.cpp

"$(INTDIR)\FSSync.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\proactive\job_thread.cpp

"$(INTDIR)\job_thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\proactive\OwnSync.cpp

"$(INTDIR)\OwnSync.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\proactive\proactive.cpp

"$(INTDIR)\proactive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "FSSync - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\Hook" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fp"$(INTDIR)\FSSync.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FSSync.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\Hook" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fp"$(INTDIR)\FSSync.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FSSync.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

CPP_SWITCHES=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fp"$(INTDIR)\FSSync.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FSSync.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /Zd /O2 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Fp"$(INTDIR)\FSSync.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FSSync.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\CommonFiles" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /D "FSDRV_AUTOTHREADREGISTER" /Fp"$(INTDIR)\FSSync.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\FSSync.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\proactive\TaskThread.cpp

"$(INTDIR)\TaskThread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\FSSync.rc

"$(INTDIR)\FSSync.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "FSSync - Win32 Release"

"FSDrvLib - Win32 Release" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Release" 
   cd "..\FSSync"

"FSDrvLib - Win32 ReleaseCLEAN" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\FSSync"

!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

"FSDrvLib - Win32 Debug" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Debug" 
   cd "..\FSSync"

"FSDrvLib - Win32 DebugCLEAN" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\FSSync"

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

"FSDrvLib - Win32 DebugS" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 DebugS" 
   cd "..\FSSync"

"FSDrvLib - Win32 DebugSCLEAN" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 DebugS" RECURSE=1 CLEAN 
   cd "..\FSSync"

!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

"FSDrvLib - Win32 ReleaseS" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 ReleaseS" 
   cd "..\FSSync"

"FSDrvLib - Win32 ReleaseSCLEAN" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 ReleaseS" RECURSE=1 CLEAN 
   cd "..\FSSync"

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

!ENDIF 


!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on mcou.dsp
!IF "$(CFG)" == ""
CFG=mcou - Win32 Debug MBCS
!MESSAGE No configuration specified. Defaulting to mcou - Win32 Debug MBCS.
!ENDIF 

!IF "$(CFG)" != "mcou - Win32 Release" && "$(CFG)" != "mcou - Win32 Debug" && "$(CFG)" != "mcou - Win32 Debug MBCS" && "$(CFG)" != "mcou - Win32 Release MBCS"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcou.mak" CFG="mcou - Win32 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mcou - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcou - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcou - Win32 Debug MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcou - Win32 Release MBCS" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "mcou - Win32 Release"

OUTDIR=.\../../../../../out/release
INTDIR=.\release
# Begin Custom Macros
OutDir=.\../../../../../out/release
# End Custom Macros

ALL : "$(OUTDIR)\mcou.dll" ".\release\mcou.pch"


CLEAN :
	-@erase "$(INTDIR)\AttachedFileEvents.obj"
	-@erase "$(INTDIR)\BinaryQueue.obj"
	-@erase "$(INTDIR)\ExchExtCallback2.obj"
	-@erase "$(INTDIR)\Hyperlinks.obj"
	-@erase "$(INTDIR)\icheck.obj"
	-@erase "$(INTDIR)\InitTerm.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\IProp.obj"
	-@erase "$(INTDIR)\mcou.obj"
	-@erase "$(INTDIR)\mcou.pch"
	-@erase "$(INTDIR)\mcou.res"
	-@erase "$(INTDIR)\MCThread.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\MessageEvents.obj"
	-@erase "$(INTDIR)\PropertySheets.obj"
	-@erase "$(INTDIR)\Register.obj"
	-@erase "$(INTDIR)\SessionEvents.obj"
	-@erase "$(INTDIR)\StartupScan.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\ToolsOptionsDlg.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mcou.dll"
	-@erase "$(OUTDIR)\mcou.exp"
	-@erase "$(OUTDIR)\mcou.lib"
	-@erase "..\..\..\..\out\release\mcou.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /Fp"$(INTDIR)\mcou.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\mcou.res" /i "..\..\..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcou.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=edkdebug.lib user32.lib Advapi32.lib mapi32.lib gdi32.lib ole32.lib /nologo /dll /incremental:no /pdb:"../../../../out/release/mcou.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /def:".\mcou.def" /out:"$(OUTDIR)\mcou.dll" /implib:"$(OUTDIR)\mcou.lib" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\ReleaseDll\\" /libpath:"..\..\..\..\cs adminkit\DEVELOPMENT\LIB" 
DEF_FILE= \
	".\mcou.def"
LINK32_OBJS= \
	"$(INTDIR)\AttachedFileEvents.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\MessageEvents.obj" \
	"$(INTDIR)\PropertySheets.obj" \
	"$(INTDIR)\SessionEvents.obj" \
	"$(INTDIR)\Hyperlinks.obj" \
	"$(INTDIR)\ToolsOptionsDlg.obj" \
	"$(INTDIR)\StartupScan.obj" \
	"$(INTDIR)\BinaryQueue.obj" \
	"$(INTDIR)\ExchExtCallback2.obj" \
	"$(INTDIR)\InitTerm.obj" \
	"$(INTDIR)\mcou.obj" \
	"$(INTDIR)\MCThread.obj" \
	"$(INTDIR)\Register.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\icheck.obj" \
	"$(INTDIR)\IProp.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\mcou.res"

"$(OUTDIR)\mcou.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"

OUTDIR=.\../../../../../out/debug
INTDIR=.\debug
# Begin Custom Macros
OutDir=.\../../../../../out/debug/
# End Custom Macros

ALL : "$(OUTDIR)\mcou.dll" ".\debug\mcou.pch" "$(OUTDIR)\mcou.bsc"


CLEAN :
	-@erase "$(INTDIR)\AttachedFileEvents.obj"
	-@erase "$(INTDIR)\AttachedFileEvents.sbr"
	-@erase "$(INTDIR)\BinaryQueue.obj"
	-@erase "$(INTDIR)\BinaryQueue.sbr"
	-@erase "$(INTDIR)\ExchExtCallback2.obj"
	-@erase "$(INTDIR)\ExchExtCallback2.sbr"
	-@erase "$(INTDIR)\Hyperlinks.obj"
	-@erase "$(INTDIR)\Hyperlinks.sbr"
	-@erase "$(INTDIR)\icheck.obj"
	-@erase "$(INTDIR)\icheck.sbr"
	-@erase "$(INTDIR)\InitTerm.obj"
	-@erase "$(INTDIR)\InitTerm.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\IProp.obj"
	-@erase "$(INTDIR)\IProp.sbr"
	-@erase "$(INTDIR)\mcou.obj"
	-@erase "$(INTDIR)\mcou.pch"
	-@erase "$(INTDIR)\mcou.res"
	-@erase "$(INTDIR)\mcou.sbr"
	-@erase "$(INTDIR)\MCThread.obj"
	-@erase "$(INTDIR)\MCThread.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\MessageEvents.obj"
	-@erase "$(INTDIR)\MessageEvents.sbr"
	-@erase "$(INTDIR)\PropertySheets.obj"
	-@erase "$(INTDIR)\PropertySheets.sbr"
	-@erase "$(INTDIR)\Register.obj"
	-@erase "$(INTDIR)\Register.sbr"
	-@erase "$(INTDIR)\SessionEvents.obj"
	-@erase "$(INTDIR)\SessionEvents.sbr"
	-@erase "$(INTDIR)\StartupScan.obj"
	-@erase "$(INTDIR)\StartupScan.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\thread.sbr"
	-@erase "$(INTDIR)\ToolsOptionsDlg.obj"
	-@erase "$(INTDIR)\ToolsOptionsDlg.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mcou.bsc"
	-@erase "$(OUTDIR)\mcou.dll"
	-@erase "$(OUTDIR)\mcou.exp"
	-@erase "$(OUTDIR)\mcou.ilk"
	-@erase "$(OUTDIR)\mcou.lib"
	-@erase "$(OUTDIR)\mcou.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mcou.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\mcou.res" /i "..\..\..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcou.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AttachedFileEvents.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\MessageEvents.sbr" \
	"$(INTDIR)\PropertySheets.sbr" \
	"$(INTDIR)\SessionEvents.sbr" \
	"$(INTDIR)\Hyperlinks.sbr" \
	"$(INTDIR)\ToolsOptionsDlg.sbr" \
	"$(INTDIR)\StartupScan.sbr" \
	"$(INTDIR)\BinaryQueue.sbr" \
	"$(INTDIR)\ExchExtCallback2.sbr" \
	"$(INTDIR)\InitTerm.sbr" \
	"$(INTDIR)\mcou.sbr" \
	"$(INTDIR)\MCThread.sbr" \
	"$(INTDIR)\Register.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\thread.sbr" \
	"$(INTDIR)\icheck.sbr" \
	"$(INTDIR)\IProp.sbr" \
	"$(INTDIR)\md5.sbr"

"$(OUTDIR)\mcou.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=edkdebug.lib user32.lib Advapi32.lib mapi32.lib gdi32.lib ole32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\mcou.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /def:".\mcou.def" /out:"$(OUTDIR)\mcou.dll" /implib:"$(OUTDIR)\mcou.lib" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll\\" /libpath:"..\..\..\..\cs adminkit\DEVELOPMENT\LIBD" 
DEF_FILE= \
	".\mcou.def"
LINK32_OBJS= \
	"$(INTDIR)\AttachedFileEvents.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\MessageEvents.obj" \
	"$(INTDIR)\PropertySheets.obj" \
	"$(INTDIR)\SessionEvents.obj" \
	"$(INTDIR)\Hyperlinks.obj" \
	"$(INTDIR)\ToolsOptionsDlg.obj" \
	"$(INTDIR)\StartupScan.obj" \
	"$(INTDIR)\BinaryQueue.obj" \
	"$(INTDIR)\ExchExtCallback2.obj" \
	"$(INTDIR)\InitTerm.obj" \
	"$(INTDIR)\mcou.obj" \
	"$(INTDIR)\MCThread.obj" \
	"$(INTDIR)\Register.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\icheck.obj" \
	"$(INTDIR)\IProp.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\mcou.res"

"$(OUTDIR)\mcou.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"

OUTDIR=.\../../../../temp/debug mbcs/mcou
INTDIR=.\../../../../temp/debug mbcs/mcou
# Begin Custom Macros
OutDir=.\../../../../temp/debug mbcs/mcou
# End Custom Macros

ALL : "..\..\..\..\out\debug mbcs\mcou.dll" "$(OUTDIR)\mcou.pch" "$(OUTDIR)\mcou.bsc"


CLEAN :
	-@erase "$(INTDIR)\AttachedFileEvents.obj"
	-@erase "$(INTDIR)\AttachedFileEvents.sbr"
	-@erase "$(INTDIR)\BinaryQueue.obj"
	-@erase "$(INTDIR)\BinaryQueue.sbr"
	-@erase "$(INTDIR)\ExchExtCallback2.obj"
	-@erase "$(INTDIR)\ExchExtCallback2.sbr"
	-@erase "$(INTDIR)\Hyperlinks.obj"
	-@erase "$(INTDIR)\Hyperlinks.sbr"
	-@erase "$(INTDIR)\icheck.obj"
	-@erase "$(INTDIR)\icheck.sbr"
	-@erase "$(INTDIR)\InitTerm.obj"
	-@erase "$(INTDIR)\InitTerm.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\IProp.obj"
	-@erase "$(INTDIR)\IProp.sbr"
	-@erase "$(INTDIR)\mcou.obj"
	-@erase "$(INTDIR)\mcou.pch"
	-@erase "$(INTDIR)\mcou.res"
	-@erase "$(INTDIR)\mcou.sbr"
	-@erase "$(INTDIR)\MCThread.obj"
	-@erase "$(INTDIR)\MCThread.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\MessageEvents.obj"
	-@erase "$(INTDIR)\MessageEvents.sbr"
	-@erase "$(INTDIR)\PropertySheets.obj"
	-@erase "$(INTDIR)\PropertySheets.sbr"
	-@erase "$(INTDIR)\Register.obj"
	-@erase "$(INTDIR)\Register.sbr"
	-@erase "$(INTDIR)\SessionEvents.obj"
	-@erase "$(INTDIR)\SessionEvents.sbr"
	-@erase "$(INTDIR)\StartupScan.obj"
	-@erase "$(INTDIR)\StartupScan.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\thread.sbr"
	-@erase "$(INTDIR)\ToolsOptionsDlg.obj"
	-@erase "$(INTDIR)\ToolsOptionsDlg.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mcou.bsc"
	-@erase "$(OUTDIR)\mcou.exp"
	-@erase "$(OUTDIR)\mcou.lib"
	-@erase "..\..\..\..\out\debug mbcs\mcou.dll"
	-@erase "..\..\..\..\out\debug mbcs\mcou.ilk"
	-@erase "..\..\..\..\out\debug mbcs\mcou.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "o:\windows\hook" /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mcou.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\mcou.res" /i "..\..\..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcou.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AttachedFileEvents.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\MessageEvents.sbr" \
	"$(INTDIR)\PropertySheets.sbr" \
	"$(INTDIR)\SessionEvents.sbr" \
	"$(INTDIR)\Hyperlinks.sbr" \
	"$(INTDIR)\ToolsOptionsDlg.sbr" \
	"$(INTDIR)\StartupScan.sbr" \
	"$(INTDIR)\BinaryQueue.sbr" \
	"$(INTDIR)\ExchExtCallback2.sbr" \
	"$(INTDIR)\InitTerm.sbr" \
	"$(INTDIR)\mcou.sbr" \
	"$(INTDIR)\MCThread.sbr" \
	"$(INTDIR)\Register.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\thread.sbr" \
	"$(INTDIR)\icheck.sbr" \
	"$(INTDIR)\IProp.sbr" \
	"$(INTDIR)\md5.sbr"

"$(OUTDIR)\mcou.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=edkdebug.lib klcsc.lib kltrace.lib user32.lib Advapi32.lib mapi32.lib gdi32.lib ole32.lib /nologo /dll /incremental:yes /pdb:"../../../../out/debug mbcs/mcou.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /def:".\mcou.def" /out:"../../../../out/debug mbcs/mcou.dll" /implib:"$(OUTDIR)\mcou.lib" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll\\" /libpath:"..\..\..\..\cs adminkit\DEVELOPMENT\NULIBD" 
DEF_FILE= \
	".\mcou.def"
LINK32_OBJS= \
	"$(INTDIR)\AttachedFileEvents.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\MessageEvents.obj" \
	"$(INTDIR)\PropertySheets.obj" \
	"$(INTDIR)\SessionEvents.obj" \
	"$(INTDIR)\Hyperlinks.obj" \
	"$(INTDIR)\ToolsOptionsDlg.obj" \
	"$(INTDIR)\StartupScan.obj" \
	"$(INTDIR)\BinaryQueue.obj" \
	"$(INTDIR)\ExchExtCallback2.obj" \
	"$(INTDIR)\InitTerm.obj" \
	"$(INTDIR)\mcou.obj" \
	"$(INTDIR)\MCThread.obj" \
	"$(INTDIR)\Register.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\icheck.obj" \
	"$(INTDIR)\IProp.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\mcou.res"

"..\..\..\..\out\debug mbcs\mcou.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"

OUTDIR=.\../../../../temp/release mbcs/mcou
INTDIR=.\../../../../temp/release mbcs/mcou
# Begin Custom Macros
OutDir=.\../../../../temp/release mbcs/mcou
# End Custom Macros

ALL : "..\..\..\..\out\release mbcs\mcou.dll" "$(OUTDIR)\mcou.pch"


CLEAN :
	-@erase "$(INTDIR)\AttachedFileEvents.obj"
	-@erase "$(INTDIR)\BinaryQueue.obj"
	-@erase "$(INTDIR)\ExchExtCallback2.obj"
	-@erase "$(INTDIR)\Hyperlinks.obj"
	-@erase "$(INTDIR)\icheck.obj"
	-@erase "$(INTDIR)\InitTerm.obj"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\IProp.obj"
	-@erase "$(INTDIR)\mcou.obj"
	-@erase "$(INTDIR)\mcou.pch"
	-@erase "$(INTDIR)\mcou.res"
	-@erase "$(INTDIR)\MCThread.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\MessageEvents.obj"
	-@erase "$(INTDIR)\PropertySheets.obj"
	-@erase "$(INTDIR)\Register.obj"
	-@erase "$(INTDIR)\SessionEvents.obj"
	-@erase "$(INTDIR)\StartupScan.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\ToolsOptionsDlg.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mcou.exp"
	-@erase "$(OUTDIR)\mcou.lib"
	-@erase "..\..\..\..\out\release mbcs\mcou.dll"
	-@erase "..\..\..\..\out\release mbcs\mcou.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "o:\windows\hook" /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /Fp"$(INTDIR)\mcou.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\mcou.res" /i "..\..\..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcou.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib Advapi32.lib mapi32.lib gdi32.lib ole32.lib /nologo /dll /incremental:no /pdb:"../../../../out/release mbcs/mcou.pdb" /debug /machine:I386 /def:".\mcou.def" /out:"../../../../out/release mbcs/mcou.dll" /implib:"$(OUTDIR)\mcou.lib" /libpath:"..\..\..\..\CommonFiles\ReleaseDll\\" /libpath:"..\..\..\..\cs adminkit\DEVELOPMENT\NULIB" 
DEF_FILE= \
	".\mcou.def"
LINK32_OBJS= \
	"$(INTDIR)\AttachedFileEvents.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\MessageEvents.obj" \
	"$(INTDIR)\PropertySheets.obj" \
	"$(INTDIR)\SessionEvents.obj" \
	"$(INTDIR)\Hyperlinks.obj" \
	"$(INTDIR)\ToolsOptionsDlg.obj" \
	"$(INTDIR)\StartupScan.obj" \
	"$(INTDIR)\BinaryQueue.obj" \
	"$(INTDIR)\ExchExtCallback2.obj" \
	"$(INTDIR)\InitTerm.obj" \
	"$(INTDIR)\mcou.obj" \
	"$(INTDIR)\MCThread.obj" \
	"$(INTDIR)\Register.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\icheck.obj" \
	"$(INTDIR)\IProp.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\mcou.res"

"..\..\..\..\out\release mbcs\mcou.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("mcou.dep")
!INCLUDE "mcou.dep"
!ELSE 
!MESSAGE Warning: cannot find "mcou.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mcou - Win32 Release" || "$(CFG)" == "mcou - Win32 Debug" || "$(CFG)" == "mcou - Win32 Debug MBCS" || "$(CFG)" == "mcou - Win32 Release MBCS"
SOURCE=.\AttachedFileEvents.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\AttachedFileEvents.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\AttachedFileEvents.obj"	"$(INTDIR)\AttachedFileEvents.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\AttachedFileEvents.obj"	"$(INTDIR)\AttachedFileEvents.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\AttachedFileEvents.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\interface.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\interface.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\interface.obj"	"$(INTDIR)\interface.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\interface.obj"	"$(INTDIR)\interface.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\interface.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MessageEvents.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\MessageEvents.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\MessageEvents.obj"	"$(INTDIR)\MessageEvents.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\MessageEvents.obj"	"$(INTDIR)\MessageEvents.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\MessageEvents.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\PropertySheets.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\PropertySheets.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\PropertySheets.obj"	"$(INTDIR)\PropertySheets.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\PropertySheets.obj"	"$(INTDIR)\PropertySheets.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\PropertySheets.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SessionEvents.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\SessionEvents.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\SessionEvents.obj"	"$(INTDIR)\SessionEvents.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\SessionEvents.obj"	"$(INTDIR)\SessionEvents.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\SessionEvents.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Hyperlinks.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\Hyperlinks.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\Hyperlinks.obj"	"$(INTDIR)\Hyperlinks.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\Hyperlinks.obj"	"$(INTDIR)\Hyperlinks.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\Hyperlinks.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ToolsOptionsDlg.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\ToolsOptionsDlg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\ToolsOptionsDlg.obj"	"$(INTDIR)\ToolsOptionsDlg.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\ToolsOptionsDlg.obj"	"$(INTDIR)\ToolsOptionsDlg.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\ToolsOptionsDlg.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\StartupScan\StartupScan.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\StartupScan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\StartupScan.obj"	"$(INTDIR)\StartupScan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\StartupScan.obj"	"$(INTDIR)\StartupScan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\StartupScan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\BinaryQueue.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\BinaryQueue.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\BinaryQueue.obj"	"$(INTDIR)\BinaryQueue.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\BinaryQueue.obj"	"$(INTDIR)\BinaryQueue.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\BinaryQueue.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ExchExtCallback2.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\ExchExtCallback2.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\ExchExtCallback2.obj"	"$(INTDIR)\ExchExtCallback2.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\ExchExtCallback2.obj"	"$(INTDIR)\ExchExtCallback2.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\ExchExtCallback2.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\InitTerm.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\InitTerm.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\InitTerm.obj"	"$(INTDIR)\InitTerm.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\InitTerm.obj"	"$(INTDIR)\InitTerm.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\InitTerm.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mcou.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\mcou.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\mcou.obj"	"$(INTDIR)\mcou.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\mcou.obj"	"$(INTDIR)\mcou.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\mcou.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MCThread.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\MCThread.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\MCThread.obj"	"$(INTDIR)\MCThread.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\MCThread.obj"	"$(INTDIR)\MCThread.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\MCThread.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Register.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\Register.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\Register.obj"	"$(INTDIR)\Register.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\Register.obj"	"$(INTDIR)\Register.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\Register.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /Fp"$(INTDIR)\mcou.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcou.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mcou.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\mcou.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "o:\windows\hook" /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mcou.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\mcou.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "o:\windows\hook" /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /I "..\..\..\..\..\windows\hook" /I "..\..\..\..\..\Prague" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MCOU_EXPORTS" /Fp"$(INTDIR)\mcou.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcou.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\util.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mcou.rc

"$(INTDIR)\mcou.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\..\..\..\..\COMMONFILES\Stuff\thread.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\thread.obj"	"$(INTDIR)\thread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\thread.obj"	"$(INTDIR)\thread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\ICheck\icheck.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\icheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\icheck.obj"	"$(INTDIR)\icheck.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\icheck.obj"	"$(INTDIR)\icheck.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\icheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\ICheck\IProp.c

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\IProp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\IProp.obj"	"$(INTDIR)\IProp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\IProp.obj"	"$(INTDIR)\IProp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\IProp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\ICheck\md5.cpp

!IF  "$(CFG)" == "mcou - Win32 Release"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug"


"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Debug MBCS"


"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mcou - Win32 Release MBCS"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 


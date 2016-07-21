# Microsoft Developer Studio Project File - Name="nPGPNts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=nPGPNts - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nPGPNts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nPGPNts.mak" CFG="nPGPNts - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nPGPNts - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nPGPNts - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nPGPNts - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\build\win32\release"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "..\..\shared\win32" /I "..\..\..\libs2\pfl\common" /I "..\..\..\libs2\pfl\win32" /I "..\..\..\libs2\pgpsdk\pub\include" /I "..\..\..\libs2\pgpsdk\win32\pub\include" /I "..\..\..\libs2\shared\external\win32\libxml2\include" /I "..\..\shared" /I "..\..\..\build" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "W32" /D "PGP_WIN32" /D "PGP_NOTES" /D WINVER=0x400 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"release/nPGPNts.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 advapi32.lib notes.lib user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /version:2.16 /subsystem:windows /dll /pdb:none /machine:I386 /out:"..\..\..\build\win32\release\ndbPGP.dll"
# Begin Custom Build
InputPath=\PgpSrc\build\win32\release\ndbPGP.dll
SOURCE="$(InputPath)"

"../../../build/win32/strings/nPGPnts.ascii.strings" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo Collecting strings. 
	if not exist ../../../build/win32/strings mkdir ../../../build/win32/strings 
	del /Q  "../../../build/win32/strings/nPGPnts.ascii.strings" 2> NUL 
	pgpdsp nPGPnts.dsp ../../../build/win32/strings/nPGPnts.ascii.strings 
	
# End Custom Build
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Building English processed strings file in UTF-8 format.	pgpstrloc -P -b8 -s ../../../build/win32/strings/nPGPnts.ascii.strings -o ../../../build/win32/strings/nPGPnts.eng.strings	rem Bulding other languages here
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nPGPNts - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\build\win32\debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /Gi /GX /ZI /Od /I "..\..\shared\win32" /I "..\..\..\libs2\pfl\common" /I "..\..\..\libs2\pfl\win32" /I "..\..\..\libs2\pgpsdk\pub\include" /I "..\..\..\libs2\pgpsdk\win32\pub\include" /I "..\..\..\libs2\shared\external\win32\libxml2\include" /I "..\..\shared" /I "..\..\..\build" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "W32" /D "PGP_WIN32" /D "PGP_NOTES" /D WINVER=0x400 /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"debug\nPGPNts.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib notes.lib user32.lib gdi32.lib comdlg32.lib shell32.lib /nologo /version:2.16 /stack:0x400,0x100 /subsystem:windows /dll /pdb:"debug\nPGPNts.pdb" /debug /machine:I386 /out:"..\..\..\build\win32\debug\ndbPGP.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=put dll with all Domino servers
PostBuild_Cmds=copy ..\..\..\build\win32\debug\ndbPGP.dll c:\Notes\main\*.*	REM copy "c:\Notes\main\ndbPGP.dll" "c:\Notes\4.5\*.*"	REM copy "c:\Notes\main\ndbPGP.dll" "c:\Notes\4.6\*.*"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "nPGPNts - Win32 Release"
# Name "nPGPNts - Win32 Debug"
# Begin Group "code"

# PROP Default_Filter ""
# Begin Group "shared PGP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\shared\win32\BlockUtils.c
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\BlockUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\DecryptVerify.c
# ADD CPP /GB /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\DecryptVerify.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\EncryptSign.c
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\EncryptSign.h
# End Source File
# Begin Source File

SOURCE=.\npgpntsstr.c
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\ParseMime.c
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\ParseMime.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\pgprc.c
# End Source File
# Begin Source File

SOURCE=..\..\shared\pgpVersionHeader.c
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\Prefs.c
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\Prefs.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\sharedstringsstr.c
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\SigEvent.c
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\SigEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\VerificationBlock.c
# ADD CPP /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\VerificationBlock.h
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\Working.c
# ADD CPP /GB /Zp8
# End Source File
# Begin Source File

SOURCE=..\..\shared\win32\Working.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\base64.c
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\FileSystemHandling.c
# End Source File
# Begin Source File

SOURCE=.\FileSystemHandling.h
# End Source File
# Begin Source File

SOURCE=.\ItemHandling.c
# End Source File
# Begin Source File

SOURCE=.\ItemHandling.h
# End Source File
# Begin Source File

SOURCE=.\LibFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\LibItem.h
# End Source File
# Begin Source File

SOURCE=".\LibLinkList.h"
# End Source File
# Begin Source File

SOURCE=.\LibNab.h
# End Source File
# Begin Source File

SOURCE=.\LibNotesMime.h
# End Source File
# Begin Source File

SOURCE=.\LibObject.h
# End Source File
# Begin Source File

SOURCE=.\LibRichText.h
# End Source File
# Begin Source File

SOURCE=.\LibString.h
# End Source File
# Begin Source File

SOURCE=.\LinkListHandling.c
# End Source File
# Begin Source File

SOURCE=.\MimeHandling.c
# End Source File
# Begin Source File

SOURCE=.\MimeHandling.h
# End Source File
# Begin Source File

SOURCE=.\NabHandling.c
# End Source File
# Begin Source File

SOURCE=.\NabHandling.h
# End Source File
# Begin Source File

SOURCE=.\ndbPGP.c
# End Source File
# Begin Source File

SOURCE=.\ndbPGP.h
# End Source File
# Begin Source File

SOURCE=.\NotesApiGlobals.c
# End Source File
# Begin Source File

SOURCE=.\NotesApiGlobals.h
# End Source File
# Begin Source File

SOURCE=.\nPGPNts.c

!IF  "$(CFG)" == "nPGPNts - Win32 Release"

# ADD CPP /GB /Od

!ELSEIF  "$(CFG)" == "nPGPNts - Win32 Debug"

# ADD CPP /GB /Fr

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\nPGPNts.h
# End Source File
# Begin Source File

SOURCE=.\ObjectHandling.c
# End Source File
# Begin Source File

SOURCE=.\ObjectHandling.h
# End Source File
# Begin Source File

SOURCE=.\PgpHandling.c
# ADD CPP /GB /Zp8
# End Source File
# Begin Source File

SOURCE=.\PgpHandling.h
# End Source File
# Begin Source File

SOURCE=.\PstGlobals.c
# End Source File
# Begin Source File

SOURCE=.\PstGlobals.h
# End Source File
# Begin Source File

SOURCE=.\RichTextHandling.c

!IF  "$(CFG)" == "nPGPNts - Win32 Release"

# ADD CPP /GB

!ELSEIF  "$(CFG)" == "nPGPNts - Win32 Debug"

# ADD CPP /GB /Zp1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RichTextHandling.h
# End Source File
# Begin Source File

SOURCE=.\StringHandling.c
# End Source File
# Begin Source File

SOURCE=.\StringHandling.h
# End Source File
# Begin Source File

SOURCE=.\VbCallable.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\nPGPNts.def
# End Source File
# Begin Source File

SOURCE=.\nPGPNts.rc
# End Source File
# End Target
# End Project

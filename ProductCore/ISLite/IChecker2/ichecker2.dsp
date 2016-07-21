# Microsoft Developer Studio Project File - Name="IChecker2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IChecker2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ichecker2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ichecker2.mak" CFG="IChecker2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IChecker2 - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IChecker2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IChecker2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/ISLite/IChecker2", HBDEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IChecker2 - Win32 ReleaseWithTrace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/ReleaseWithTrace"
# PROP BASE Intermediate_Dir "../../../temp/ReleaseWithTrace/prague/ISLite/IChecker2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/ReleaseWithTrace"
# PROP Intermediate_Dir "../../../temp/ReleaseWithTrace/prague/ISLite/IChecker2"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:"../../../out/ReleaseWithTrace/ichecker2.ppl" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\ReleaseWithTrace\ichecker2.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "IChecker2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/ISLite/IChecker2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/ISLite/IChecker2"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib chkstk.obj libcmt.lib /nologo /base:"0x64400000" /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:"../../../out/Release/ichk2.ppl" /opt:ref
# SUBTRACT LINK32 /pdb:none /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\ichk2.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "IChecker2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/ISLite/IChecker2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/ISLite/IChecker2"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/ichk2.ppl" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "IChecker2 - Win32 ReleaseWithTrace"
# Name "IChecker2 - Win32 Release"
# Name "IChecker2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\calc_file_hash.c
# End Source File
# Begin Source File

SOURCE=.\ichecker2.c
# End Source File
# Begin Source File

SOURCE=.\plugin_ichecker2.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\calc_file_hash.h
# End Source File
# Begin Source File

SOURCE=.\ENTR_ELF.H
# End Source File
# Begin Source File

SOURCE=.\forcedbgout.h
# End Source File
# Begin Source File

SOURCE=.\ichecker2.h
# End Source File
# Begin Source File

SOURCE=".\ichecker2_res.h"
# End Source File
# Begin Source File

SOURCE=.\itss.h
# End Source File
# Begin Source File

SOURCE=.\NEWEXE.H
# End Source File
# Begin Source File

SOURCE=.\ole2.h
# End Source File
# Begin Source File

SOURCE=.\PEHDR.H
# End Source File
# Begin Source File

SOURCE=.\plugin_ichecker2.h
# End Source File
# Begin Source File

SOURCE=.\rar.h
# End Source File
# Begin Source File

SOURCE=.\zip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ichecker2.imp
# End Source File
# Begin Source File

SOURCE=.\ichecker2.meta
# End Source File
# Begin Source File

SOURCE=".\ichecker2_res.rc"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ichecker2.dep
# End Source File
# Begin Source File

SOURCE=.\ichecker2.mak
# End Source File
# Begin Source File

SOURCE=..\..\Include\Prt\IChecker2.prt
# End Source File
# End Target
# End Project

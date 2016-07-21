# Microsoft Developer Studio Project File - Name="KLGuard2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=KLGuard2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KLGuard2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KLGuard2.mak" CFG="KLGuard2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KLGuard2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "KLGuard2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "KLGuard2 - Win32 Release with dbgout" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/TestProjects/KLGuard2", RCQIAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KLGuard2 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 sign.lib fsdrvlib.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"..\..\..\..\CommonFiles\ReleaseDll" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\Release\KLGuard2.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KLGuard2 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "UNICODE" /D "_UNICODE" /YX"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sign.lib fsdrvlib.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll"
# Begin Special Build Tool
TargetPath=.\Debug\KLGuard2.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KLGuard2 - Win32 Release with dbgout"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "KLGuard2___Win32_Release_with_dbgout"
# PROP BASE Intermediate_Dir "KLGuard2___Win32_Release_with_dbgout"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "KLGuard2___Win32_Release_with_dbgout"
# PROP Intermediate_Dir "KLGuard2___Win32_Release_with_dbgout"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /D "__DBGOUT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 sign.lib fsdrvlib.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"..\..\..\..\CommonFiles\ReleaseDll" /fixed:no
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 sign.lib fsdrvlib.lib Version.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /libpath:"..\..\..\..\CommonFiles\ReleaseDll" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\KLGuard2___Win32_Release_with_dbgout\KLGuard2.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "KLGuard2 - Win32 Release"
# Name "KLGuard2 - Win32 Debug"
# Name "KLGuard2 - Win32 Release with dbgout"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoPTCHAR.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\DGenericSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\DHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\DKnownProcesses.cpp
# End Source File
# Begin Source File

SOURCE=.\DrvEventList.cpp
# End Source File
# Begin Source File

SOURCE=.\DSandBoxSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\DSandBoxWarnings.cpp
# End Source File
# Begin Source File

SOURCE=.\FileLog.cpp
# End Source File
# Begin Source File

SOURCE=.\FilePlacement.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\Job_SandBox_Warnings.cpp
# End Source File
# Begin Source File

SOURCE=.\jobs.cpp
# End Source File
# Begin Source File

SOURCE=.\KLGuard2.cpp
# End Source File
# Begin Source File

SOURCE=.\KLGuard2.rc
# End Source File
# Begin Source File

SOURCE=.\KLGuard2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\KnownProcessList.cpp
# End Source File
# Begin Source File

SOURCE=.\Layout.c
# End Source File
# Begin Source File

SOURCE=.\ListColumnPositions.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\OwnSync.cpp
# End Source File
# Begin Source File

SOURCE=.\Processes.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessRecognizer.cpp
# End Source File
# Begin Source File

SOURCE=.\Reestr.cpp
# End Source File
# Begin Source File

SOURCE=.\RegAutoRuns.cpp
# End Source File
# Begin Source File

SOURCE=.\servfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StrHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ThCheckSelfCopy.cpp
# End Source File
# Begin Source File

SOURCE=.\ThCMD_Remote.cpp
# End Source File
# Begin Source File

SOURCE=.\ThModifiedProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\ThNewProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\UncontrolledFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\WinFileInfo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppSettings.h
# End Source File
# Begin Source File

SOURCE=.\AutoPTCHAR.h
# End Source File
# Begin Source File

SOURCE=.\CheckWindow.h
# End Source File
# Begin Source File

SOURCE=.\DAbout.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\DefList.h
# End Source File
# Begin Source File

SOURCE=.\DGenericSettings.h
# End Source File
# Begin Source File

SOURCE=.\DHistory.h
# End Source File
# Begin Source File

SOURCE=.\DKnownProcesses.h
# End Source File
# Begin Source File

SOURCE=.\DrvEventList.h
# End Source File
# Begin Source File

SOURCE=.\DSandBoxSettings.h
# End Source File
# Begin Source File

SOURCE=.\DSandBoxWarnings.h
# End Source File
# Begin Source File

SOURCE=.\FileLog.h
# End Source File
# Begin Source File

SOURCE=.\FilePlacement.h
# End Source File
# Begin Source File

SOURCE=.\Common\hyperlink.h
# End Source File
# Begin Source File

SOURCE=.\Job_SandBox_Warnings.h
# End Source File
# Begin Source File

SOURCE=.\jobs.h
# End Source File
# Begin Source File

SOURCE=.\KLGuard2.h
# End Source File
# Begin Source File

SOURCE=.\KLGuard2Dlg.h
# End Source File
# Begin Source File

SOURCE=.\KnownProcessList.h
# End Source File
# Begin Source File

SOURCE=.\Layout.h
# End Source File
# Begin Source File

SOURCE=.\ListColumnPositions.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\OwnSync.h
# End Source File
# Begin Source File

SOURCE=.\Processes.h
# End Source File
# Begin Source File

SOURCE=.\ProcessRecognizer.h
# End Source File
# Begin Source File

SOURCE=.\Reestr.h
# End Source File
# Begin Source File

SOURCE=.\RegAutoRuns.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\servfuncs.h
# End Source File
# Begin Source File

SOURCE=.\servicestuff.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StrHistory.h
# End Source File
# Begin Source File

SOURCE=.\TaskThread.h
# End Source File
# Begin Source File

SOURCE=.\ThCheckSelfCopy.h
# End Source File
# Begin Source File

SOURCE=.\ThCMD_Remote.h
# End Source File
# Begin Source File

SOURCE=.\ThModifiedProcess.h
# End Source File
# Begin Source File

SOURCE=.\ThNewProcess.h
# End Source File
# Begin Source File

SOURCE=.\UncontrolledFiles.h
# End Source File
# Begin Source File

SOURCE=.\WinFileInfo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\icon_ale.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_san.ico
# End Source File
# Begin Source File

SOURCE=.\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\KL.ico
# End Source File
# Begin Source File

SOURCE=.\res\KLGuard1.ico
# End Source File
# Begin Source File

SOURCE=.\res\KLGuard2.ico
# End Source File
# Begin Source File

SOURCE=.\res\KLGuard2.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\KLGuard2.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

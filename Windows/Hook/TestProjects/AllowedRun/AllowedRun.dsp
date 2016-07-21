# Microsoft Developer Studio Project File - Name="AllowedRun" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AllowedRun - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AllowedRun.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AllowedRun.mak" CFG="AllowedRun - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AllowedRun - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AllowedRun - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/TestProjects/AllowedRun", KIHEAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AllowedRun - Win32 Release"

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
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Win32Utils.lib FSDrvLib.lib sign.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\Release/AllowedRun.exe" /libpath:"..\..\..\..\CommonFiles\ReleaseDll"
# Begin Special Build Tool
TargetPath=\WINDOWS\HOOK\Release\AllowedRun.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AllowedRun - Win32 Debug"

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
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Win32Utils.lib FSDrvLib.lib sign.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"..\..\Debug/AllowedRun.exe" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll"
# Begin Special Build Tool
TargetPath=\WINDOWS\HOOK\Debug\AllowedRun.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AllowedRun - Win32 Release"
# Name "AllowedRun - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AllowedRun.cpp
# End Source File
# Begin Source File

SOURCE=.\AllowedRun.rc
# End Source File
# Begin Source File

SOURCE=.\AllowedRunDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DApp.cpp
# End Source File
# Begin Source File

SOURCE=.\DApplications.cpp
# End Source File
# Begin Source File

SOURCE=.\DChanges.cpp
# End Source File
# Begin Source File

SOURCE=.\DFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\DFolders.cpp
# End Source File
# Begin Source File

SOURCE=.\EvCache.cpp
# End Source File
# Begin Source File

SOURCE=.\FiltersArray.cpp
# End Source File
# Begin Source File

SOURCE=.\FiltersStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\OwnTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AllowedRun.h
# End Source File
# Begin Source File

SOURCE=.\AllowedRunDlg.h
# End Source File
# Begin Source File

SOURCE=.\DApp.h
# End Source File
# Begin Source File

SOURCE=.\DApplications.h
# End Source File
# Begin Source File

SOURCE=.\DChanges.h
# End Source File
# Begin Source File

SOURCE=.\DFolder.h
# End Source File
# Begin Source File

SOURCE=.\DFolders.h
# End Source File
# Begin Source File

SOURCE=.\EvCache.h
# End Source File
# Begin Source File

SOURCE=.\FiltersArray.h
# End Source File
# Begin Source File

SOURCE=.\FiltersStorage.h
# End Source File
# Begin Source File

SOURCE=.\OwnTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AllowedRun.ico
# End Source File
# Begin Source File

SOURCE=.\res\AllowedRun.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\AllowedRun.dep
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AllowedRun.mak
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

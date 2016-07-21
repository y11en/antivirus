# Microsoft Developer Studio Project File - Name="IGuard" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=IGuard - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IGuard.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IGuard.mak" CFG="IGuard - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IGuard - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IGuard - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Interface/IGuard", DVUBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IGuard - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\..\CommonFiles" /I "..\..\..\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Win32Utils.lib Shlwapi.lib FSDrvLib.lib sign.lib KLKeyRec.lib DTUtils.lib Property.lib KLDTSer.lib SWM.lib WinAvpIO.lib imagehlp.lib AVPControls.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\Release/IGuard.dll" /libpath:"..\..\..\..\CommonFiles\ReleaseDll" /libpath:"../../../../out/release"
# Begin Special Build Tool
TargetPath=\Windows\Hook\Release\IGuard.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "IGuard - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\CommonFiles" /I "..\..\..\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Win32Utils.lib Shlwapi.lib FSDrvLib.lib sign.lib KLKeyRec.lib DTUtils.lib Property.lib KLDTSer.lib SWM.lib WinAvpIO.lib imagehlp.lib AVPControls.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Debug/IGuard.dll" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll" /libpath:"../../../../out/debug"
# Begin Special Build Tool
TargetPath=\Windows\Hook\Debug\IGuard.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "IGuard - Win32 Release"
# Name "IGuard - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DAppID.cpp
# End Source File
# Begin Source File

SOURCE=.\DClientProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\DConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\DEventInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\DFilters.cpp
# End Source File
# Begin Source File

SOURCE=.\DHexInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DInfoForUser.cpp
# End Source File
# Begin Source File

SOURCE=.\Dparameter.cpp
# End Source File
# Begin Source File

SOURCE=.\DWarningNoFilters.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\EvCache.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\FiltersArray.cpp
# End Source File
# Begin Source File

SOURCE=.\FiltersStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\IGuard.cpp
# End Source File
# Begin Source File

SOURCE=.\IGuard.def
# End Source File
# Begin Source File

SOURCE=.\IGuard.rc
# End Source File
# Begin Source File

SOURCE=.\OwnTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Params.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PSAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DAppID.h
# End Source File
# Begin Source File

SOURCE=.\DClientProperty.h
# End Source File
# Begin Source File

SOURCE=.\DConfig.h
# End Source File
# Begin Source File

SOURCE=.\DEventInfo.h
# End Source File
# Begin Source File

SOURCE=.\DFilter.h
# End Source File
# Begin Source File

SOURCE=.\DFilters.h
# End Source File
# Begin Source File

SOURCE=.\DHexInfo.h
# End Source File
# Begin Source File

SOURCE=.\DInfo.h
# End Source File
# Begin Source File

SOURCE=.\DInfoForUser.h
# End Source File
# Begin Source File

SOURCE=.\Dparameter.h
# End Source File
# Begin Source File

SOURCE=.\DWarningNoFilters.h
# End Source File
# Begin Source File

SOURCE=.\EnumProcess.h
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

SOURCE=.\IGuard.h
# End Source File
# Begin Source File

SOURCE=.\OwnTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Params.h
# End Source File
# Begin Source File

SOURCE=.\PSAPI.h
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

SOURCE=.\res\IGuard.rc2
# End Source File
# Begin Source File

SOURCE=.\res\kav.bmp
# End Source File
# Begin Source File

SOURCE=.\res\KL89X21.BMP
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

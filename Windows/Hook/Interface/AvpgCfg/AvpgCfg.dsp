# Microsoft Developer Studio Project File - Name="AvpgCfg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AvpgCfg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AvpgCfg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvpgCfg.mak" CFG="AvpgCfg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvpgCfg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AvpgCfg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Interface/AvpgCfg", WAABAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AvpgCfg - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\AVPControls" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 AVPControls.lib Property.lib DTUtils.lib Win32Utils.lib Shlwapi.lib FSDrvLib.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\Release/AvpgCfg.dll" /libpath:"..\..\..\..\CommonFiles\ReleaseDll\\"

!ELSEIF  "$(CFG)" == "AvpgCfg - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\..\AVPControls" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 AVPControls.lib Property.lib DTUtils.lib Win32Utils.lib Shlwapi.lib FSDrvLib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Debug/AvpgCfg.dll" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll\\"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "AvpgCfg - Win32 Release"
# Name "AvpgCfg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AvpgCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\AvpgCfg.def
# End Source File
# Begin Source File

SOURCE=.\AvpgCfg.rc
# End Source File
# Begin Source File

SOURCE=.\coolbarctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DAddNewFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\DAuditWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\DChangeAppID.cpp
# End Source File
# Begin Source File

SOURCE=.\DClientInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\DDefaultFilters.cpp
# End Source File
# Begin Source File

SOURCE=.\DEditParam.cpp
# End Source File
# Begin Source File

SOURCE=.\DListMode.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DLog.cpp
# End Source File
# Begin Source File

SOURCE=.\DTreeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\DTreeMode.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\FiltersArray.cpp
# End Source File
# Begin Source File

SOURCE=.\FiltersStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterTreeInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Localize.cpp
# End Source File
# Begin Source File

SOURCE=.\PSAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\Reestr.cpp
# End Source File
# Begin Source File

SOURCE=.\reportviewctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AvpgCfg.h
# End Source File
# Begin Source File

SOURCE=.\coolbarctrl.h
# End Source File
# Begin Source File

SOURCE=.\DAddNewFilter.h
# End Source File
# Begin Source File

SOURCE=.\DAuditWizard.h
# End Source File
# Begin Source File

SOURCE=.\DChangeAppID.h
# End Source File
# Begin Source File

SOURCE=.\DClientInfo.h
# End Source File
# Begin Source File

SOURCE=.\DConfig.h
# End Source File
# Begin Source File

SOURCE=.\DDefaultFilters.h
# End Source File
# Begin Source File

SOURCE=.\DEditParam.h
# End Source File
# Begin Source File

SOURCE=.\DListMode.h
# End Source File
# Begin Source File

SOURCE=.\DLog.h
# End Source File
# Begin Source File

SOURCE=.\DTreeBase.h
# End Source File
# Begin Source File

SOURCE=.\DTreeMode.h
# End Source File
# Begin Source File

SOURCE=.\EnumProcess.h
# End Source File
# Begin Source File

SOURCE=.\FiltersArray.h
# End Source File
# Begin Source File

SOURCE=.\FiltersStorage.h
# End Source File
# Begin Source File

SOURCE=.\FilterTreeInfo.h
# End Source File
# Begin Source File

SOURCE=.\Localize.h
# End Source File
# Begin Source File

SOURCE=.\PSAPI.h
# End Source File
# Begin Source File

SOURCE=.\Reestr.h
# End Source File
# Begin Source File

SOURCE=.\reportviewctrl.h
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

SOURCE=.\AVPG.rc2
# End Source File
# Begin Source File

SOURCE=.\res\CELC_Ctrl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\chdeffil_.ico
# End Source File
# Begin Source File

SOURCE=.\res\Ctrlimag.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dynamic.ico
# End Source File
# Begin Source File

SOURCE=.\res\listv.ico
# End Source File
# Begin Source File

SOURCE=.\res\log.ico
# End Source File
# Begin Source File

SOURCE=.\res\treev.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section AvpgCfg : {66102C00-6BFE-42E2-B105-2019F5AEBACC}
# 	2:14:GridListCtrl.h:GridListCtrl.h
# 	2:16:GridListCtrl.cpp:GridListCtrl.cpp
# 	2:20:CLASS: CGridListCtrl:CGridListCtrl
# 	2:19:Application Include:avpgcfg.h
# End Section
# Section AvpgCfg : {DD54D565-2FCA-11D3-96B0-00104B5B66AA}
# 	2:5:Class:CReportViewCtrl
# 	2:10:HeaderFile:reportviewctrl.h
# 	2:8:ImplFile:reportviewctrl.cpp
# End Section
# Section AvpgCfg : {DD54D567-2FCA-11D3-96B0-00104B5B66AA}
# 	2:21:DefaultSinkHeaderFile:reportviewctrl.h
# 	2:16:DefaultSinkClass:CReportViewCtrl
# End Section

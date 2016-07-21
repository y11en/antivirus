# Microsoft Developer Studio Project File - Name="mcou_antispam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mcou_antispam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mcou_antispam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcou_antispam.mak" CFG="mcou_antispam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mcou_antispam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcou_antispam - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/OutlookPlugin/mcou_antispam", UKISAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mcou_antispam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\..\out\Debug\"
# PROP Intermediate_Dir "..\..\..\..\..\Temp\Debug\ppp\mcou_antispam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\basegui" /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_DEBUGSTR" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\..\..\out\Debug\mcouas.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "mcou_antispam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mcou_antispam___Win32_Release"
# PROP BASE Intermediate_Dir "mcou_antispam___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\..\out\Release\"
# PROP Intermediate_Dir "..\..\..\..\..\Temp\Release\ppp\mcou_antispam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /O1 /I "O:\PPP\Include\wtl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\..\..\..\basegui" /I "..\..\..\..\Include" /I "..\..\..\..\..\Prague\Include" /I "..\..\..\..\..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_DEBUGSTR" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib /nologo /base:"0x65200000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\..\..\out\Release\mcouas.dll" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\mcouas.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "mcou_antispam - Win32 Debug"
# Name "mcou_antispam - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Addin.cpp
# End Source File
# Begin Source File

SOURCE=.\AntispamOUSupportClass.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\dlldatax.c
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\explorerhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\handler.cpp
# End Source File
# Begin Source File

SOURCE=.\OutlookAddin.cpp
# End Source File
# Begin Source File

SOURCE=.\OutlookAddin.def
# End Source File
# Begin Source File

SOURCE=.\OutlookAddin.idl
# ADD MTL /tlb ".\OutlookAddin.tlb" /h "OutlookAddin.h" /iid "OutlookAddin_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\PageAntispam.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Addin.h
# End Source File
# Begin Source File

SOURCE=.\AntispamOUSupportClass.h
# End Source File
# Begin Source File

SOURCE=.\dlldatax.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\explorerhandler.h
# End Source File
# Begin Source File

SOURCE=.\handler.h
# End Source File
# Begin Source File

SOURCE=.\PageAntispam.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Addin.rgs
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BtnGreen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BtnRed.bmp
# End Source File
# Begin Source File

SOURCE=.\mcou_antispam.rc
# End Source File
# Begin Source File

SOURCE=.\res\PageAntispam.rgs
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Stuff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\COMMONFILES\Stuff\thread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\COMMONFILES\Stuff\thread.h
# End Source File
# End Group
# Begin Group "BackgroundScan"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BackgroundScan\BackgroundScan.cpp
# End Source File
# Begin Source File

SOURCE=.\BackgroundScan\BackgroundScan.h
# End Source File
# End Group
# Begin Group "Graylist"

# PROP Default_Filter ""
# Begin Group "SimpleMessage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GrayList\SimpleMessage\MAPIHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\GrayList\SimpleMessage\MAPIHelper.h
# End Source File
# Begin Source File

SOURCE=.\GrayList\SimpleMessage\SimpleMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\GrayList\SimpleMessage\SimpleMessage.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\GrayList\GrayList.cpp
# End Source File
# End Group
# Begin Group "trace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DViTraceSupport.cpp
# End Source File
# Begin Source File

SOURCE=.\DViTraceSupport.h
# End Source File
# End Group
# Begin Group "ExchRule"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ExchRule\ExchRule.cpp
# End Source File
# Begin Source File

SOURCE=.\ExchRule\ExchRule.h
# End Source File
# Begin Source File

SOURCE=.\ExchRule\Register.cpp
# End Source File
# Begin Source File

SOURCE=.\ExchRule\Register.h
# End Source File
# End Group
# End Target
# End Project
# Section OutlookAddin : {00000000-0000-0000-0000-800000800000}
# 	1:12:IDD_LITEPAGE:112
# 	1:12:IDR_LITEPAGE:111
# 	1:12:IDB_LITEPAGE:110
# End Section

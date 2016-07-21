# Microsoft Developer Studio Project File - Name="CKAHUMTESTUI2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CKAHUMTESTUI2 - Win32 Debug_Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CKAHUMTESTUI2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CKAHUMTESTUI2.mak" CFG="CKAHUMTESTUI2 - Win32 Debug_Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CKAHUMTESTUI2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CKAHUMTESTUI2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "CKAHUMTESTUI2 - Win32 Debug_Unicode" (based on "Win32 (x86) Application")
!MESSAGE "CKAHUMTESTUI2 - Win32 Release_Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Components/Windows/KAH/Test/CKAHUMTESTUI2", IQHLAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CKAHUMTESTUI2 - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../../Windows/Hook" /I "../../../../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"../../../../../out/Release/CKAHUMTESTUI2.exe"

!ELSEIF  "$(CFG)" == "CKAHUMTESTUI2 - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../../Windows/Hook" /I "../../../../../CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../../../out/Debug/CKAHUMTESTUI2.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "CKAHUMTESTUI2 - Win32 Debug_Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CKAHUMTESTUI2___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "CKAHUMTESTUI2___Win32_Debug_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../../Windows/Hook" /I "../../../../../CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../../../../out/Debug/CKAHUMTESTUI2.exe" /pdbtype:sept
# ADD LINK32 ws2_32.lib ckahum.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../../../../out/Debug/CKAHUMTESTUI2.exe" /pdbtype:sept /libpath:"../../../../../out/Debug/"

!ELSEIF  "$(CFG)" == "CKAHUMTESTUI2 - Win32 Release_Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CKAHUMTESTUI2___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "CKAHUMTESTUI2___Win32_Release_Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../../Windows/Hook" /I "../../../../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"../../../../../out/Release/CKAHUMTESTUI2.exe"
# ADD LINK32 ws2_32.lib ckahum.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../../../../../out/Release/CKAHUMTESTUI2.exe" /libpath:"../../../../../out/Release/"

!ENDIF 

# Begin Target

# Name "CKAHUMTESTUI2 - Win32 Release"
# Name "CKAHUMTESTUI2 - Win32 Debug"
# Name "CKAHUMTESTUI2 - Win32 Debug_Unicode"
# Name "CKAHUMTESTUI2 - Win32 Release_Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActiveConnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AddressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ApplicationRuleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ApplicationRuleElementDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ApplicationRulesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BanometDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHUMTESTUI2.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHUMTESTUI2.rc
# End Source File
# Begin Source File

SOURCE=.\CKAHUMTESTUI2Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CRConsoleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CRConsoleEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\HostStatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewHostAddrDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketRuleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketRulesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PortDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RdrExcl.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleTimeEltDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleTimesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ActiveConnDlg.h
# End Source File
# Begin Source File

SOURCE=.\AddressDlg.h
# End Source File
# Begin Source File

SOURCE=.\ApplicationRuleDlg.h
# End Source File
# Begin Source File

SOURCE=.\ApplicationRuleElementDlg.h
# End Source File
# Begin Source File

SOURCE=.\ApplicationRulesDlg.h
# End Source File
# Begin Source File

SOURCE=.\BanometDlg.h
# End Source File
# Begin Source File

SOURCE=.\CKAHUMTESTUI2.h
# End Source File
# Begin Source File

SOURCE=.\CKAHUMTESTUI2Dlg.h
# End Source File
# Begin Source File

SOURCE=.\CRConsoleDlg.h
# End Source File
# Begin Source File

SOURCE=.\CRConsoleEdit.h
# End Source File
# Begin Source File

SOURCE=.\HostStatDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewHostAddrDlg.h
# End Source File
# Begin Source File

SOURCE=.\PacketRuleDlg.h
# End Source File
# Begin Source File

SOURCE=.\PacketRulesDlg.h
# End Source File
# Begin Source File

SOURCE=.\PortDlg.h
# End Source File
# Begin Source File

SOURCE=.\RdrExcl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RuleTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\RuleTimeEltDlg.h
# End Source File
# Begin Source File

SOURCE=.\RuleTimesDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\CKAHUMTESTUI2.ico
# End Source File
# Begin Source File

SOURCE=.\res\CKAHUMTESTUI2.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\ckahumtestui2.manifest
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

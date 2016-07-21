# Microsoft Developer Studio Project File - Name="OEAntiSpam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OEAntiSpam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OEAntiSpam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OEAntiSpam.mak" CFG="OEAntiSpam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OEAntiSpam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OEAntiSpam - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/OutlookExpress/OEAntiSpam", BNEMBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OEAntiSpam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../out/Debug"
# PROP Intermediate_Dir "../../../../../temp/Debug/oespam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../CSShare" /I "../../../../../prague/include" /I "../../../../include" /I "../../../../basegui" /I "../../../../../commonfiles" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "OEANTISPAM_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fsdrvlib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../../../out/Debug/oeas.dll" /pdbtype:sept /libpath:"../../../../../Out/Debug"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "OEAntiSpam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OEAntiSpam___Win32_Release"
# PROP BASE Intermediate_Dir "OEAntiSpam___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../out/release"
# PROP Intermediate_Dir "../../../../../temp/Release/oespam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OEANTISPAM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "../../../../CSShare" /I "../../../../../prague/include" /I "../../../../include" /I "../../../../basegui" /I "../../../../../commonfiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "OEANTISPAM_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /out:"ReleaseMinSize/oeas.dll" /pdbtype:sept /RELEASE /OPT:NOWIN98
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib fsdrvlib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../../../out/Release/oeas.dll" /libpath:"../../../../../Out/Release" /RELEASE /OPT:NOWIN98
# SUBTRACT LINK32 /pdb:none /map

!ENDIF 

# Begin Target

# Name "OEAntiSpam - Win32 Debug"
# Name "OEAntiSpam - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\TheBatPlugin\CriticalSection.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MyComModule.cpp
# End Source File
# Begin Source File

SOURCE=.\OEAntiSpam.cpp
# End Source File
# Begin Source File

SOURCE=.\OEAntiSpam.def
# End Source File
# Begin Source File

SOURCE=.\OEAntiSpam.rc
# End Source File
# Begin Source File

SOURCE=.\OERules.cpp
# End Source File
# Begin Source File

SOURCE=.\OESettings.cpp
# End Source File
# Begin Source File

SOURCE=.\OETricks.cpp
# End Source File
# Begin Source File

SOURCE=..\..\TheBatPlugin\PragueHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RebarPos.cpp
# End Source File
# Begin Source File

SOURCE=.\RebarWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\TheBatPlugin\remote_mbl.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Windows\Hook\R3Hook\sputnik.c
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StringCollector.cpp
# End Source File
# Begin Source File

SOURCE=.\SubclassedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SubclassedCopyMoveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SubclassedCreateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SubclassedMsgPropDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SubclassedMsgSourceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SWMRG.cpp
# End Source File
# Begin Source File

SOURCE=.\Tracer.cpp
# End Source File
# Begin Source File

SOURCE=.\Utility.cpp
# End Source File
# Begin Source File

SOURCE=.\WndInterceptor.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\TheBatPlugin\CriticalSection.h
# End Source File
# Begin Source File

SOURCE=..\..\TheBatAntispam\EnsureCleanup.h
# End Source File
# Begin Source File

SOURCE=..\..\OutlookPlugin\mcou_antispam\GuiLoader.h
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\MyComModule.h
# End Source File
# Begin Source File

SOURCE=.\OEAntiSpam.h
# End Source File
# Begin Source File

SOURCE=.\OERules.h
# End Source File
# Begin Source File

SOURCE=.\OESettings.h
# End Source File
# Begin Source File

SOURCE=.\OETricks.h
# End Source File
# Begin Source File

SOURCE=..\..\TheBatPlugin\PragueHelper.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\RebarPos.h
# End Source File
# Begin Source File

SOURCE=.\RebarWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\TheBatPlugin\remote_mbl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\Windows\Hook\R3Hook\sputnik.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StringCollector.h
# End Source File
# Begin Source File

SOURCE=.\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=.\SubclassedBase.h
# End Source File
# Begin Source File

SOURCE=.\SubclassedCopyMoveDlg.h
# End Source File
# Begin Source File

SOURCE=.\SubclassedCreateDlg.h
# End Source File
# Begin Source File

SOURCE=.\SubclassedMsgPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\SubclassedMsgSourceDlg.h
# End Source File
# Begin Source File

SOURCE=.\SWMRG.h
# End Source File
# Begin Source File

SOURCE=.\Tracer.h
# End Source File
# Begin Source File

SOURCE=.\Utility.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=.\WndInterceptor.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BtnGreen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BtnRed.bmp
# End Source File
# Begin Source File

SOURCE=.\res\config.ico
# End Source File
# Begin Source File

SOURCE=.\res\kav.ico
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="LicensingTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=LicensingTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LicensingTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LicensingTest.mak" CFG="LicensingTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LicensingTest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "LicensingTest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Licensing2/test/func/build/win32", VNKVBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LicensingTest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "o:\CommonFiles\Licensing2\include" /I "..\LicensingTestApi" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 dskm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LicensingTestApi.lib licensing.lib property.lib kldtser.lib swm.lib winavpio.lib win32utils.lib winavpff.lib /nologo /subsystem:console /machine:I386 /libpath:"o:\out\ReleaseDll" /libpath:"o:\out\Release" /libpath:"..\..\..\api\build\win32\Release"

!ELSEIF  "$(CFG)" == "LicensingTest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "o:\CommonFiles\Licensing2\include" /I "..\LicensingTestApi" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dskm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LicensingTestApi.lib licensing.lib property.lib kldtser.lib swm.lib winavpio.lib win32utils.lib winavpff.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"o:\out\Debug" /libpath:"..\..\..\api\build\win32\Debug"

!ENDIF 

# Begin Target

# Name "LicensingTest - Win32 Release"
# Name "LicensingTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\LicensingTest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ProgramClientInfo_Helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_CheckLicenseRestrictions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_GetInstalledLicenseKeys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_InitializeLicensing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_InstallLicenseKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_ParseLicenseKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_SetContext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_UninstallLicenseKey.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\Constants.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ProgramClientInfo_Helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_CheckLicenseRestrictions.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_GetInstalledLicenseKeys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_InitializeLicensing.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_InstallLicenseKey.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_ParseLicenseKey.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_SetContext.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Test_UninstallLicenseKey.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

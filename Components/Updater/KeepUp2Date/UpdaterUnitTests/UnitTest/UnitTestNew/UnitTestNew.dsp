# Microsoft Developer Studio Project File - Name="UnitTestNew" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=UnitTestNew - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UnitTestNew.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnitTestNew.mak" CFG="UnitTestNew - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnitTestNew - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "UnitTestNew - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "UnitTestNew"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UnitTestNew - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "UnitTestNew - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\client" /I "..\..\..\\" /I "..\..\..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "LIBS_STATIC_LINKING" /D "LAX_NO_IMPLEMENTATION" /D "KAVUPDCORE_STATIC_CONFIGURATION" /D _WIN32_WINNT=0x0500 /D "SECURITY_WIN32" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib wininet.lib advapi32.lib user32.lib slib_core.lib Iphlpapi.lib libboost_unit_test_framework-vc6-mt.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBCMTD.lib" /pdbtype:sept /libpath:"..\..\..\client\iupdater\bin.debug" /libpath:"..\..\..\..\..\out\debug" /libpath:"..\..\..\..\..\out\debugs" /libpath:"..\..\..\UpdaterUnitTests\lib" /libpath:"..\..\..\..\..\cs adminkit\development2\lib\debug"

!ENDIF 

# Begin Target

# Name "UnitTestNew - Win32 Release"
# Name "UnitTestNew - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\callback.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpServer.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IniFile\IniBasedRollback.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IniFile\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\logger.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\Client\helper\map_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\TestFtpTransport.cpp
# End Source File
# Begin Source File

SOURCE=.\TestHttpProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\TestHttpTransport.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuite.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteAddress.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteBoth.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteNet.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteProxyDetector.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteRetr.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteSiteInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteSmallClasses.cpp
# End Source File
# Begin Source File

SOURCE=.\TestSuiteTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\boost.h
# End Source File
# Begin Source File

SOURCE=.\callback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Client\include\callback_iface.h
# End Source File
# Begin Source File

SOURCE=.\constants.h
# End Source File
# Begin Source File

SOURCE=.\FtpServer.h
# End Source File
# Begin Source File

SOURCE=.\HttpServer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\IniFile\IniBasedRollback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\IniFile\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\journal.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\logger.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\TestFtpTransport.h
# End Source File
# Begin Source File

SOURCE=.\TestHttpProxy.h
# End Source File
# Begin Source File

SOURCE=.\TestHttpTransport.h
# End Source File
# Begin Source File

SOURCE=.\TestSuite.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteAddress.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteBase.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteNet.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteProxyDetector.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteSiteInfo.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteSmallClasses.h
# End Source File
# Begin Source File

SOURCE=.\TestSuiteTransaction.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

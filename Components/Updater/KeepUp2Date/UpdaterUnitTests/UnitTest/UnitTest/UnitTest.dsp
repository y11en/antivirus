# Microsoft Developer Studio Project File - Name="UnitTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=UnitTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UnitTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnitTest.mak" CFG="UnitTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnitTest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "UnitTest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "UnitTest"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UnitTest - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "LIBS_STATIC_LINKING" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "UnitTest - Win32 Debug"

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
# ADD CPP /nologo /ML /W3 /Gm /GX /ZI /Od /I "..\..\..\client" /I "..\..\..\\" /I "..\..\..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "LIBS_STATIC_LINKING" /D "LAX_NO_IMPLEMENTATION" /D "KAVUPDCORE_STATIC_CONFIGURATION" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib slib_core.lib ws2_32.lib wininet.lib shlwapi.lib sign.lib klcsftb.lib libboost_unit_test_framework-vc6-s.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBCMTD.lib" /pdbtype:sept /libpath:"..\..\..\client\iupdater\bin.debug" /libpath:"..\..\..\..\..\out\debugs" /libpath:"..\..\..\UpdaterUnitTests\lib" /libpath:"..\..\..\..\..\cs adminkit\development2\lib\debug"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "UnitTest - Win32 Release"
# Name "UnitTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\helper.cpp
# End Source File
# Begin Source File

SOURCE=.\ini_helper.cpp
# End Source File
# Begin Source File

SOURCE=.\ini_updater.cpp
# End Source File
# Begin Source File

SOURCE=.\ini_updater_lists.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\prod_config.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_CHTTP.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_CNetCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_IndexFileXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_LocalFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_NetCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_SitesFileXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_UpdateInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Test_UpdaterTransaction.cpp
# End Source File
# Begin Source File

SOURCE=.\TestInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\upd_config.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\boost_test.h
# End Source File
# Begin Source File

SOURCE=.\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\helper.h
# End Source File
# Begin Source File

SOURCE=.\ini_helper.h
# End Source File
# Begin Source File

SOURCE=.\ini_updater.h
# End Source File
# Begin Source File

SOURCE=.\ini_updater_lists.h
# End Source File
# Begin Source File

SOURCE=.\prod_config.h
# End Source File
# Begin Source File

SOURCE=.\Test_CHTTP.h
# End Source File
# Begin Source File

SOURCE=.\Test_CNetCore.h
# End Source File
# Begin Source File

SOURCE=.\Test_IndexFileXMLParser.h
# End Source File
# Begin Source File

SOURCE=.\Test_LocalFile.h
# End Source File
# Begin Source File

SOURCE=.\Test_NetCore.h
# End Source File
# Begin Source File

SOURCE=.\Test_SitesFileXMLParser.h
# End Source File
# Begin Source File

SOURCE=.\Test_UpdateInfo.h
# End Source File
# Begin Source File

SOURCE=.\Test_UpdaterTransaction.h
# End Source File
# Begin Source File

SOURCE=.\TestInterface.h
# End Source File
# Begin Source File

SOURCE=.\upd_config.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

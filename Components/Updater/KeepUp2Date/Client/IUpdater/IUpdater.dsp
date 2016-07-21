# Microsoft Developer Studio Project File - Name="IUpdater" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=IUpdater - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IUpdater.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IUpdater.mak" CFG="IUpdater - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IUpdater - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "IUpdater - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "IUpdater"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IUpdater - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../" /I "../../../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "LIBS_STATIC_LINKING" /FR /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../" /i "../../../../CommonFiles/" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 slib_core.lib klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib wininet.lib advapi32.lib user32.lib iphlpapi.lib /nologo /subsystem:console /pdb:"../../../../out/Release/IUpdater.pdb" /debug /machine:I386 /out:"../../../../out/Release/IUpdater.exe" /pdbtype:sept /libpath:"../../../../out/Release" /libpath:"../../../../CS AdminKit/development2/lib/release/"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "IUpdater - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /I "../../../../CommonFiles" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "LIBS_STATIC_LINKING" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../" /i "../../../../CommonFiles/" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 slib_core.lib klcsftb.lib property.lib swm.lib dskm.lib sign.lib ws2_32.lib wininet.lib advapi32.lib user32.lib iphlpapi.lib /nologo /subsystem:console /pdb:"../../../../out/Debug/IUpdater.pdb" /debug /machine:I386 /out:"../../../../out/Debug/IUpdater.exe" /pdbtype:sept /libpath:"../../../../out/Debug" /libpath:"../../../../CS AdminKit/development2/lib/debug/"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "IUpdater - Win32 Release"
# Name "IUpdater - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\DumpWriter\dumpWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\ini_journal.cpp
# End Source File
# Begin Source File

SOURCE=.\ini_log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IniFile\IniBasedRollback.cpp
# End Source File
# Begin Source File

SOURCE=.\iniCallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\IniConfiguration.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IniFile\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IUpdater.cpp
# End Source File
# Begin Source File

SOURCE=.\IUpdater.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\DumpWriter\dumpWriter.h
# End Source File
# Begin Source File

SOURCE=.\ini_journal.h
# End Source File
# Begin Source File

SOURCE=.\ini_log.h
# End Source File
# Begin Source File

SOURCE=..\..\IniFile\IniBasedRollback.h
# End Source File
# Begin Source File

SOURCE=.\iniCallbacks.h
# End Source File
# Begin Source File

SOURCE=.\IniConfiguration.h
# End Source File
# Begin Source File

SOURCE=..\..\IniFile\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\include\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

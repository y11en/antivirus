# Microsoft Developer Studio Project File - Name="licutest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=licutest - Win32 DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "licutest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "licutest.mak" CFG="licutest - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "licutest - Win32 ReleaseDll" (based on "Win32 (x86) Console Application")
!MESSAGE "licutest - Win32 DebugDll" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Licensing2/test/unit/build/win32", XNKVBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "licutest - Win32 ReleaseDll"

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
# ADD CPP /nologo /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "licutest - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "O:\cppunit-1.10.2\include" /I "O:\CommonFiles\Licensing2\include" /I "O:\CommonFiles\Licensing2\src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"precompiled.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib cppunitd.lib licensing.lib swm.lib property.lib kldtser.lib win32utils.lib winavpio.lib winavpff.lib dskm.lib /nologo /subsystem:console /debug /machine:I386 /out:"debug/licutestd.exe" /pdbtype:sept /libpath:"O:\cppunit-1.10.2\lib" /libpath:"O:\CommonFiles\Licensing2\build\win32\Debug" /libpath:"o:\out\debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "licutest - Win32 ReleaseDll"
# Name "licutest - Win32 DebugDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\Main.cpp

!IF  "$(CFG)" == "licutest - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "licutest - Win32 DebugDll"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\precompiled.cpp

!IF  "$(CFG)" == "licutest - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "licutest - Win32 DebugDll"

# ADD CPP /Yc"precompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\TestControlDb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TestExpression.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TestIndex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TestKeyOperations.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLicenseContext.cpp

!IF  "$(CFG)" == "licutest - Win32 ReleaseDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "licutest - Win32 DebugDll"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\TestLicensing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLicensingFactory.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLobjectWrapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\TestStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\utility\WinFileStorageR.cpp

!IF  "$(CFG)" == "licutest - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "licutest - Win32 DebugDll"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\utility\WinFileStorageRW.cpp

!IF  "$(CFG)" == "licutest - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "licutest - Win32 DebugDll"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\precompiled.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestControlDb.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestExpression.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestKeyOperations.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLicenseContext.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLicensing.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLicensingFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestLobjectWrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\TestStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\utility\WinFileStorageR.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\utility\WinFileStorageRW.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

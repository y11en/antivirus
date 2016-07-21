# Microsoft Developer Studio Project File - Name="kav50" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=kav50 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kav50.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kav50.mak" CFG="kav50 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kav50 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "kav50 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/personal/kav50", YPRGAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kav50 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/kav50"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "../include" /I "../../prague/include" /I "../../updater60/include" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 prremote.lib kernel32.lib fssync.lib sign.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../out/Release/avp.exe" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\avp.exe
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "kav50 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/kav50"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../updater60/include" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 prremote.lib kernel32.lib fssync.lib sign.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../out/Debug/avp.exe" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kav50 - Win32 Release"
# Name "kav50 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BinTrace\bintrace.c
# End Source File
# Begin Source File

SOURCE=.\exchndl.cpp
# End Source File
# Begin Source File

SOURCE=.\kav50.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\NTService.cpp
# End Source File
# Begin Source File

SOURCE=.\parsecmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Service\SA.CPP
# End Source File
# Begin Source File

SOURCE=.\shell.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BinTrace\bintrace.h
# End Source File
# Begin Source File

SOURCE=.\cmdln.h
# End Source File
# Begin Source File

SOURCE=.\exchndl.h
# End Source File
# Begin Source File

SOURCE=.\hostmsgs.h
# End Source File
# Begin Source File

SOURCE=.\kav50.h
# End Source File
# Begin Source File

SOURCE=.\NTService.h
# End Source File
# Begin Source File

SOURCE=.\NTServiceEventLogMsg.h
# End Source File
# Begin Source File

SOURCE=.\parsecmd.h
# End Source File
# Begin Source File

SOURCE=.\shell.h
# End Source File
# Begin Source File

SOURCE=.\shellinc.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_ppp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\kav50.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\kav50.rc
# End Source File
# Begin Source File

SOURCE=..\RES\main_on.ico
# End Source File
# Begin Source File

SOURCE=.\MSG00000.bin
# End Source File
# Begin Source File

SOURCE=.\NTServiceEventLogMsg.rc2
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\kav50.dep
# End Source File
# Begin Source File

SOURCE=.\kav50.mak
# End Source File
# Begin Source File

SOURCE=..\Install\ppp.reg
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

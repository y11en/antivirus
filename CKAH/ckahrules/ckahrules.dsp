# Microsoft Developer Studio Project File - Name="ckahrules" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ckahrules - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ckahrules.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ckahrules.mak" CFG="ckahrules - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ckahrules - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ckahrules - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/Kaspersky Anti-Virus Personal 5.0 MP1/Components/Windows/KAH/ckahrules", CQHLAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ckahrules - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\temp\Release\ckahrules"
# PROP Intermediate_Dir "..\..\..\..\temp\Release\ckahrules"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CKAHRULES_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GR /GX /Zi /O2 /I "../../../../Windows/Hook/" /I "../../../../CommonFiles" /I "..\..\..\..\klava\kernel\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CKAHRULES_EXPORTS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib FSDrvLib.lib advapi32.lib rpcrt4.lib User32.lib Ws2_32.lib win32utils.lib version.lib iphlpapi.lib msvcrt.lib /nologo /base:"0x6A200000" /dll /pdb:"..\..\..\..\out\Release\ckahrule.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\..\..\..\out\Release\ckahrule.dll" /implib:"..\..\..\..\out\Release\ckahrule.lib" /libpath:"../../../../CommonFiles/ReleaseDll" /libpath:"../../../../Out/Release" /opt:ref
# SUBTRACT LINK32 /pdb:none /nodefaultlib /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\ckahrule.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=call tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ckahrules - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\temp\Debug\ckahrules"
# PROP Intermediate_Dir "..\..\..\..\temp\Debug\ckahrules"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CKAHRULES_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../../Windows/Hook/" /I "../../../../CommonFiles" /I "..\..\..\..\klava\kernel\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "CKAHRULES_EXPORTS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib FSDrvLib.lib advapi32.lib rpcrt4.lib User32.lib Ws2_32.lib win32utils.lib version.lib iphlpapi.lib /nologo /base:"0x6A200000" /dll /pdb:"..\..\..\..\out\Debug\ckahrule.pdb" /debug /machine:I386 /out:"..\..\..\..\out\Debug\ckahrule.dll" /implib:"..\..\..\..\out\Debug\ckahrule.lib" /pdbtype:sept /libpath:"../../../../CommonFiles/DebugDll" /libpath:"../../../../Out/Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ckahrules - Win32 Release"
# Name "ckahrules - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Firewall"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ckahrules.cpp
# End Source File
# Begin Source File

SOURCE=.\ckahrulesint.h
# End Source File
# End Group
# Begin Group "IFace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\ckahrules.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ckahmain.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ckahmain.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ckahrules.RC
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonFiles\Version\ver_mod.rc2
# End Source File
# End Group
# End Target
# End Project

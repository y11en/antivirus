# Microsoft Developer Studio Project File - Name="TRUE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TRUE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TRUE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TRUE.mak" CFG="TRUE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TRUE - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TRUE - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/TRUE", XSMBAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "TRUE - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /GX /O2 /I "$(BASEDIR)\inc" /I "o:\windows\hook" /D "_MBCS" /D "_LIB" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D _DLL=1 /D _X86_=1 /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=PostBuild.cmd Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Zi /Od /Gy /I "$(BASEDIR2k)\inc" /I "$(BASEDIR2k)\inc\ddk" /I "$(CPU)\\" /I "o:\windows\hook" /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_LIB" /D "_DEBUG" /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D _DLL=1 /D _X86_=1 /D _DBG_INFO_LEVEL=$(INFODEBUGLEVEL) /D _AVPG_UNLOADABLE=$(AVPG_UNLOADABLE) /FR /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=PostBuild.cmd Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "TRUE - Win32 Release"
# Name "TRUE - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Protocols"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Protocols\ARP.c
# End Source File
# Begin Source File

SOURCE=.\Protocols\ARP.h
# End Source File
# Begin Source File

SOURCE=.\Protocols\Eth.c
# End Source File
# Begin Source File

SOURCE=.\Protocols\Eth.h
# End Source File
# Begin Source File

SOURCE=.\Protocols\Icmp.c
# End Source File
# Begin Source File

SOURCE=.\Protocols\icmp.h
# End Source File
# Begin Source File

SOURCE=.\Protocols\IPv4.c
# End Source File
# Begin Source File

SOURCE=.\Protocols\IPv4.h
# End Source File
# Begin Source File

SOURCE=.\Protocols\Tcp.c
# End Source File
# Begin Source File

SOURCE=.\Protocols\Tcp.h
# End Source File
# Begin Source File

SOURCE=.\Protocols\Udp.c
# End Source File
# Begin Source File

SOURCE=.\Protocols\Udp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FilterEvent.c
# End Source File
# Begin Source File

SOURCE=.\NtBased.c
# End Source File
# Begin Source File

SOURCE=.\StdAfx.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Common.h
# End Source File
# Begin Source File

SOURCE=.\ErrCodes.h
# End Source File
# Begin Source File

SOURCE=.\FilterEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\commonfiles\Hook\NETID.h
# End Source File
# Begin Source File

SOURCE=.\NetParser.h
# End Source File
# Begin Source File

SOURCE=.\NtBased.h
# End Source File
# Begin Source File

SOURCE=.\ParseTables.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TrueDebug.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\PostBuild.cmd
# End Source File
# Begin Source File

SOURCE=.\TRUE.dep
# End Source File
# Begin Source File

SOURCE=.\TRUE.mak
# End Source File
# End Target
# End Project

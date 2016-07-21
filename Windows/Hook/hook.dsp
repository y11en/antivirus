# Microsoft Developer Studio Project File - Name="hook" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=hook - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hook.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hook.mak" CFG="hook - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hook - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "hook - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/hook", SWZAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hook - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "$(AlternateInterface)" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib advapi32.lib user32.lib comctl32.lib shell32.lib gdi32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/avpg.exe"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib

!ELSEIF  "$(CFG)" == "hook - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "$(AlternateInterface)" /D _DBG_INFO_LEVEL=$(INFODEBUGLEVEL) /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib advapi32.lib user32.lib comctl32.lib shell32.lib gdi32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/avpg.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /incremental:no /map

!ENDIF 

# Begin Target

# Name "hook - Win32 Release"
# Name "hook - Win32 Debug"
# Begin Source File

SOURCE=.\client.c
# End Source File
# Begin Source File

SOURCE=.\COMMON.C
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\debug.c

!IF  "$(CFG)" == "hook - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "hook - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\defs.h
# End Source File
# Begin Source File

SOURCE=".\Driver AVPG.doc"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\filter.c
# End Source File
# Begin Source File

SOURCE=.\HOOK.CPP
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\InvThread.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ioctl.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\logging.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MemMap.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Ntcommon.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\osspec.c
# End Source File
# Begin Source File

SOURCE=.\osspec.h
# End Source File
# Begin Source File

SOURCE=.\PPage.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\r3.c

!IF  "$(CFG)" == "hook - Win32 Release"

!ELSEIF  "$(CFG)" == "hook - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res.rc
# End Source File
# Begin Source File

SOURCE=.\structures.h
# End Source File
# Begin Source File

SOURCE=.\syscalls.c

!IF  "$(CFG)" == "hook - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "hook - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ThSubPro.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ThSubPro~.c

!IF  "$(CFG)" == "hook - Win32 Release"

!ELSEIF  "$(CFG)" == "hook - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TSPv2.c

!IF  "$(CFG)" == "hook - Win32 Release"

!ELSEIF  "$(CFG)" == "hook - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Vxdcommn.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project

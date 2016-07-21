# Microsoft Developer Studio Project File - Name="ipc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ipc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ipc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ipc.mak" CFG="ipc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ipc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ipc - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /O1 /I "../../gcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib /nologo /base:"0x62500000" /subsystem:windows /dll /pdb:"\release\ipc.dll.pdb" /map:"\release\ipc.dll.map" /machine:I386 /out:"\release\ipc.dll" /implib:"\release\ipc.lib" /pdbtype:con /opt:ref
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ipc - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GX /Zi /Od /I "../../gcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib /nologo /base:"0x62500000" /subsystem:windows /dll /pdb:"\debug\ipc.dll.pdb" /debug /machine:I386 /out:"\debug\ipc.dll" /implib:"\debug\ipc.lib" /pdbtype:con
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ipc - Win32 Release"
# Name "ipc - Win32 Debug"
# Begin Source File

SOURCE=..\..\gcommon\CommonHeader.h
# End Source File
# Begin Source File

SOURCE=..\..\gcommon\DateTime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\gcommon\DateTime.h
# End Source File
# Begin Source File

SOURCE=..\..\gcommon\HandlesTable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\gcommon\HandlesTable.h
# End Source File
# Begin Source File

SOURCE=.\ipc.cpp
# End Source File
# Begin Source File

SOURCE=.\ipc.def
# End Source File
# Begin Source File

SOURCE=.\ipc.h
# End Source File
# Begin Source File

SOURCE=.\ipc.rc
# End Source File
# Begin Source File

SOURCE=.\ipc32.cpp
# End Source File
# Begin Source File

SOURCE=.\ipc32.h
# End Source File
# Begin Source File

SOURCE=.\ipc_def.h
# End Source File
# Begin Source File

SOURCE=.\ipc_err.h
# End Source File
# Begin Source File

SOURCE=.\ipcBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcBuffer.h
# End Source File
# Begin Source File

SOURCE=.\ipcConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcConnection.h
# End Source File
# Begin Source File

SOURCE=.\ipcInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcInfo.h
# End Source File
# Begin Source File

SOURCE=.\ipcServer.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcServer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\gcommon\sa.cpp
# End Source File
# Begin Source File

SOURCE=..\..\gcommon\sa.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Target
# End Project

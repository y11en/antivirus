# Microsoft Developer Studio Project File - Name="klavsyslib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=klavsyslib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klavsyslib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klavsyslib.mak" CFG="klavsyslib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klavsyslib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "klavsyslib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "klavsyslib"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klavsyslib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\Release"
# PROP Intermediate_Dir "..\..\..\Temp\Release\klava\kernel\klavsyslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Zi /O2 /I "..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "klavsyslib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\..\Temp\Debug\klava\kernel\klavsyslib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Zi /Od /I "..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "klavsyslib - Win32 Release"
# Name "klavsyslib - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\db_load.cpp
# End Source File
# Begin Source File

SOURCE=.\dso_api.cpp
# End Source File
# Begin Source File

SOURCE=.\io_api.cpp
# End Source File
# Begin Source File

SOURCE=.\kfb_file_loader.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_loader.cpp
# End Source File
# Begin Source File

SOURCE=.\os_vm.cpp
# End Source File
# Begin Source File

SOURCE=.\sync_api.cpp
# End Source File
# Begin Source File

SOURCE=.\tmpfile_api.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\include\klav_io.h
# End Source File
# Begin Source File

SOURCE=..\include\klav_props.h
# End Source File
# Begin Source File

SOURCE=..\include\klavcore.h
# End Source File
# Begin Source File

SOURCE=..\include\klavdef.h
# End Source File
# Begin Source File

SOURCE=..\include\klavsys.h
# End Source File
# Begin Source File

SOURCE=..\include\klavsys_os.h
# End Source File
# Begin Source File

SOURCE=..\include\klavsys_unix.h
# End Source File
# Begin Source File

SOURCE=..\include\klavsys_windows.h
# End Source File
# End Group
# Begin Group "unix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\unix\unix_alloc.cpp
# End Source File
# Begin Source File

SOURCE=.\unix\unix_dso.cpp
# End Source File
# Begin Source File

SOURCE=.\unix\unix_err.cpp
# End Source File
# Begin Source File

SOURCE=.\unix\unix_io.cpp
# End Source File
# Begin Source File

SOURCE=.\unix\unix_io_old.cpp
# End Source File
# Begin Source File

SOURCE=.\unix\unix_sync.cpp
# End Source File
# End Group
# Begin Group "windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\windows\win_alloc.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_api.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_dso.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_err.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_io.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_io_lib.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_sync.cpp
# End Source File
# Begin Source File

SOURCE=.\windows\win_tmpfile.cpp
# End Source File
# End Group
# End Target
# End Project

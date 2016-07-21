# Microsoft Developer Studio Project File - Name="klavsys" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=klavsys - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klavsys.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klavsys.mak" CFG="klavsys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klavsys - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klavsys - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "klavsys"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klavsys - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\Release"
# PROP Intermediate_Dir "..\..\..\Temp\Release\klava\kernel\klavsys"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVSYS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Zi /O2 /I "..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVSYS_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\include" /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 klsys.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /libpath:"..\..\..\Out\Release" /opt:ref /opt:icf
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "klavsys - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\..\Temp\Debug\klava\kernel\klavsys"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVSYS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLAVSYS_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\include" /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 klsys.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /libpath:"..\..\..\Out\Debug"

!ENDIF 

# Begin Target

# Name "klavsys - Win32 Release"
# Name "klavsys - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\klavsyslib\db_load.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\dso_api.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\io_api.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\kfb_file_loader.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\mod_loader.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\os_vm.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\sync_api.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\tmpfile_api.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
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
# Begin Group "resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "unix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\klavsyslib\unix\unix_alloc.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\unix\unix_dso.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\unix\unix_err.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\unix\unix_io.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\unix\unix_io_old.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\unix\unix_sync.cpp
# End Source File
# End Group
# Begin Group "windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\klavsyslib\windows\win_alloc.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_api.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_dso.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_err.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_io.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_io_lib.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_sync.cpp
# End Source File
# Begin Source File

SOURCE=..\klavsyslib\windows\win_tmpfile.cpp
# End Source File
# End Group
# End Target
# End Project

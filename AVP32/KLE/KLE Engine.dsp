# Microsoft Developer Studio Project File - Name="KLE Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=KLE Engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KLE Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KLE Engine.mak" CFG="KLE Engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KLE Engine - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "KLE Engine - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/KLE", FQMBAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "KLE Engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f "KLE Engine.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "KLE Engine.exe"
# PROP BASE Bsc_Name "KLE Engine.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake -f Engine.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "KLE_E.ZIP"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "KLE Engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f "KLE Engine.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "KLE Engine.exe"
# PROP BASE Bsc_Name "KLE Engine.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake -f Engine.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "KLE_E.ZIP"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "KLE Engine - Win32 Release"
# Name "KLE Engine - Win32 Debug"

!IF  "$(CFG)" == "KLE Engine - Win32 Release"

!ELSEIF  "$(CFG)" == "KLE Engine - Win32 Debug"

!ENDIF 

# Begin Source File

SOURCE=.\Engine.mak
# End Source File
# Begin Source File

SOURCE=.\files.lst
# End Source File
# Begin Source File

SOURCE=.\KLEAPI.h
# End Source File
# Begin Source File

SOURCE=.\README.TXT
# End Source File
# End Target
# End Project

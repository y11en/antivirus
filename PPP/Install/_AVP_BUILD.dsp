# Microsoft Developer Studio Project File - Name="_AVP_BUILD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=_AVP_BUILD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "_AVP_BUILD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "_AVP_BUILD.mak" CFG="_AVP_BUILD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "_AVP_BUILD - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "_AVP_BUILD - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "_AVP_BUILD - Win32 ReleaseVSS" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/install", DFHHAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "_AVP_BUILD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "_AVP_BUILD___Win32_Release"
# PROP BASE Intermediate_Dir "_AVP_BUILD___Win32_Release"
# PROP BASE Cmd_Line "NMAKE /f _AVP_BUILD.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "_AVP_BUILD.exe"
# PROP BASE Bsc_Name "_AVP_BUILD.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "_AVP_BUILD___Win32_Release"
# PROP Intermediate_Dir "_AVP_BUILD___Win32_Release"
# PROP Cmd_Line "nmake /f "_AVP_BUILD.mak" BUILD=Release"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "_AVP_BUILD.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "_AVP_BUILD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "_AVP_BUILD___Win32_Debug"
# PROP BASE Intermediate_Dir "_AVP_BUILD___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f _AVP_BUILD.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "_AVP_BUILD.exe"
# PROP BASE Bsc_Name "_AVP_BUILD.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "_AVP_BUILD___Win32_Debug"
# PROP Intermediate_Dir "_AVP_BUILD___Win32_Debug"
# PROP Cmd_Line "nmake /f "_AVP_BUILD.mak" BUILD=Debug"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "_AVP_BUILD.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "_AVP_BUILD - Win32 ReleaseVSS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "_AVP_BUILD___Win32_ReleaseVSS"
# PROP BASE Intermediate_Dir "_AVP_BUILD___Win32_ReleaseVSS"
# PROP BASE Cmd_Line "nmake /f "_AVP_BUILD.mak" BUILD=Release"
# PROP BASE Rebuild_Opt "rebuild REBUILD=/REBUILD"
# PROP BASE Target_File "_AVP_BUILD.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "_AVP_BUILD___Win32_ReleaseVSS"
# PROP Intermediate_Dir "_AVP_BUILD___Win32_ReleaseVSS"
# PROP Cmd_Line "nmake /f "_AVP_BUILD.mak" BUILD=Release BuildType_VSSLabelOnly="1""
# PROP Rebuild_Opt "rebuild REBUILD=/REBUILD"
# PROP Target_File "_AVP_BUILD.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "_AVP_BUILD - Win32 Release"
# Name "_AVP_BUILD - Win32 Debug"
# Name "_AVP_BUILD - Win32 ReleaseVSS"

!IF  "$(CFG)" == "_AVP_BUILD - Win32 Release"

!ELSEIF  "$(CFG)" == "_AVP_BUILD - Win32 Debug"

!ELSEIF  "$(CFG)" == "_AVP_BUILD - Win32 ReleaseVSS"

!ENDIF 

# Begin Source File

SOURCE=.\_AVP_BUILD.MAK
# End Source File
# Begin Source File

SOURCE=.\avp_version.mak
# End Source File
# Begin Source File

SOURCE=.\get_http_content.pl
# End Source File
# Begin Source File

SOURCE=..\..\out\Release\hist.txt
# End Source File
# End Target
# End Project

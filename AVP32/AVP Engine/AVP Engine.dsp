# Microsoft Developer Studio Project File - Name="AVP Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=AVP ENGINE - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AVP Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AVP Engine.mak" CFG="AVP ENGINE - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVP Engine - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "AVP Engine - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/AVP Engine", UREAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "AVP Engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "NMAKE -f Engine.mak"
# PROP BASE Rebuild_Opt "-a"
# PROP BASE Target_File "../Release/avp_e.zip"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "nmake -f engine.mak"
# PROP Rebuild_Opt "-a"
# PROP Target_File "../Release/avp_e.zip"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "AVP Engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "nmake -f engine.mak"
# PROP BASE Rebuild_Opt "-a"
# PROP BASE Target_File "../Release/avp_e.zip"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "nmake -f engine.mak"
# PROP Rebuild_Opt "-a"
# PROP Target_File "../Release/avp_e.zip"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "AVP Engine - Win32 Release"
# Name "AVP Engine - Win32 Debug"

!IF  "$(CFG)" == "AVP Engine - Win32 Release"

!ELSEIF  "$(CFG)" == "AVP Engine - Win32 Debug"

!ENDIF 

# Begin Source File

SOURCE="..\..\!Read.me\Avp_engine_package.txt"
# End Source File
# Begin Source File

SOURCE=.\Engine.mak
# End Source File
# Begin Source File

SOURCE=.\ENGINE.RTF
# End Source File
# Begin Source File

SOURCE=.\README.TXT
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_df.h
# End Source File
# End Target
# End Project

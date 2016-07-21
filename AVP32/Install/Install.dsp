# Microsoft Developer Studio Project File - Name="Install" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Install - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Install.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Install.mak" CFG="Install - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Install - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Install - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/Install", YHPIAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Install - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Install.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Install.exe"
# PROP BASE Bsc_Name "Install.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "Install.mak" BUILD=Release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Install.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Install.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Install.exe"
# PROP BASE Bsc_Name "Install.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "Install.mak" BUILD=Debug"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Install.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Install - Win32 Release"
# Name "Install - Win32 Debug"

!IF  "$(CFG)" == "Install - Win32 Release"

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\avp.txt
# End Source File
# Begin Source File

SOURCE=.\Install.mak
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\0004B81E.key
# End Source File
# Begin Source File

SOURCE=.\AVP.INI
# End Source File
# Begin Source File

SOURCE=.\ppp.bmp
# End Source File
# Begin Source File

SOURCE=.\ppp_small.bmp
# End Source File
# End Group
# End Target
# End Project

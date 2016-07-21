# Microsoft Developer Studio Project File - Name="Dos AVP_STUB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Dos AVP_STUB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Dos AVP_STUB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dos AVP_STUB.mak" CFG="Dos AVP_STUB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dos AVP_STUB - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Dos AVP_STUB - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/AVP_32", YJEAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Dos AVP_STUB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f "Dos AVP_STUB.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Dos AVP_STUB.exe"
# PROP BASE Bsc_Name "Dos AVP_STUB.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f"avp_stub.mak" CFG=..\..\out\release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\..\out\release\avp_stub.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Dos AVP_STUB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f "Dos AVP_STUB.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Dos AVP_STUB.exe"
# PROP BASE Bsc_Name "Dos AVP_STUB.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f"avp_stub.mak" CFG=..\..\out\debug"
# PROP Rebuild_Opt "/a"
# PROP Target_File "..\..\out\debug\avp_stub.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Dos AVP_STUB - Win32 Release"
# Name "Dos AVP_STUB - Win32 Debug"

!IF  "$(CFG)" == "Dos AVP_STUB - Win32 Release"

!ELSEIF  "$(CFG)" == "Dos AVP_STUB - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Avp_stub.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Avp_stub.mak
# End Source File
# End Target
# End Project

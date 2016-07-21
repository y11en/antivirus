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
# PROP Scc_ProjName ""$/PPP/Install", DFHHAAAA"
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
# PROP Rebuild_Opt ""
# PROP Target_File "Installer"
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
# PROP Rebuild_Opt ""
# PROP Target_File "Installer"
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

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;reg"
# Begin Group "ru"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Localize\Russian\CustomMessages.txt

!IF  "$(CFG)" == "Install - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Localize\Russian\License.txt
# End Source File
# Begin Source File

SOURCE=.\Localize\Russian\Russian.isl
# End Source File
# End Group
# Begin Group "en"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Localize\English\CustomMessages.txt

!IF  "$(CFG)" == "Install - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Install - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Localize\English\English.isl
# End Source File
# Begin Source File

SOURCE=.\Localize\English\License.txt
# End Source File
# End Group
# Begin Group "ppp.reg"

# PROP Default_Filter "reg"
# Begin Source File

SOURCE=.\ppp.reg
# End Source File
# Begin Source File

SOURCE=.\ppp_fs.reg
# End Source File
# Begin Source File

SOURCE=.\ppp_kav.reg
# End Source File
# Begin Source File

SOURCE=.\ppp_kis.reg
# End Source File
# Begin Source File

SOURCE=.\ppp_wks.reg
# End Source File
# End Group
# Begin Source File

SOURCE=.\Install.mak
# End Source File
# Begin Source File

SOURCE=.\ppp_AHFWRuleDefines.reg
# End Source File
# Begin Source File

SOURCE=.\ppp_regisrty_update.bat
# End Source File
# Begin Source File

SOURCE=.\ppp_remove.reg
# End Source File
# Begin Source File

SOURCE=.\ppp_settings.reg
# End Source File
# Begin Source File

SOURCE=.\RegGrps.reg
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\FwPresets.ini
# End Source File
# Begin Source File

SOURCE=.\res\install_kav.bmp
# End Source File
# Begin Source File

SOURCE=.\res\install_kis.bmp
# End Source File
# Begin Source File

SOURCE=".\res\kavpers-uninst.ico"
# End Source File
# Begin Source File

SOURCE=.\res\main_inst.ico
# End Source File
# Begin Source File

SOURCE=.\res\ppp_small_04.bmp
# End Source File
# Begin Source File

SOURCE=.\regmonitor.ini
# End Source File
# Begin Source File

SOURCE=.\res\setup2.ico
# End Source File
# End Group
# End Target
# End Project

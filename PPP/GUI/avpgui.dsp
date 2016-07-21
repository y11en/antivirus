# Microsoft Developer Studio Project File - Name="avpgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=avpgui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "avpgui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avpgui.mak" CFG="avpgui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avpgui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avpgui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "avpgui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/avpgui"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_PRAGUE_TRACE_" /D "XTREE_MEM_PRAGUE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\include" /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 basegui.lib /nologo /base:"0x62000000" /dll /debug /machine:I386 /def:".\avpgui.def" /out:"../../out/Release/avpgui.ppl" /libpath:"..\..\CommonFiles\msvcrt42" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/Release" /opt:ref
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\Projects\KAVP7\out\Release\avpgui.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/avpgui"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XTREE_MEM_PRAGUE" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\include" /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 basegui.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug/avpgui.ppl" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/Debug"
# SUBTRACT LINK32 /pdb:none /map
# Begin Custom Build - Performing registration
OutDir=.\../../out/Debug
TargetPath=\Projects\KAVP7\out\Debug\avpgui.ppl
InputPath=\Projects\KAVP7\out\Debug\avpgui.ppl
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build
# Begin Special Build Tool
ProjDir=.
TargetDir=\Projects\KAVP7\out\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy "$(ProjDir)\skin" "$(TargetDir)\skin" /Y /R /S /I
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "avpgui - Win32 Release"
# Name "avpgui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "COM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\avpgui.idl

!IF  "$(CFG)" == "avpgui - Win32 Release"

# ADD MTL /h "comgui.h" /iid "avpgui_i.c" /Oicf
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# ADD MTL /h "comgui.h" /iid "avpgui_i.c"
# SUBTRACT MTL /mktyplib203

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\basegui.idl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ComAvpGui.cpp
# End Source File
# Begin Source File

SOURCE=.\ComAvpGui.h
# End Source File
# Begin Source File

SOURCE=.\ComBaseGui.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\AlertDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\avpgui.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"

# ADD CPP /I "../include/licensing" /Yc"stdafx.h"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\avpgui.def

!IF  "$(CFG)" == "avpgui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FWRules.cpp
# End Source File
# Begin Source File

SOURCE=.\ListsView.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\NewsReader.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_gui.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"

# ADD CPP /I "../include/licensing"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProductInfoCab.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfilesView.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\RootSink.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\ServiceSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\ServicesView.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\Upgrade\ss.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProfileProduct.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskView.cpp
# End Source File
# Begin Source File

SOURCE=.\Test.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreatsView.cpp
# End Source File
# Begin Source File

SOURCE=.\tmclient.cpp
# End Source File
# Begin Source File

SOURCE=.\TrayIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\UpgradeFrom46.cpp
# End Source File
# Begin Source File

SOURCE=.\UpgradeFrom50.cpp
# End Source File
# Begin Source File

SOURCE=.\Wizards.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Serializable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\structs\s_ah.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ahfw.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ahstm.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_antidial.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_antiphishing.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_antispam.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_as.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_as_trainsupport.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_as_trainwizard.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_avs.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_bb.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_bl.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_gui.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_httpprotocoller.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_httpscan.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_ids.h
# End Source File
# Begin Source File

SOURCE=..\..\prague\Include\structs\s_ipresolver.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_licensing.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_licmgr.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_mailwasher.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mc.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mc_oe.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mc_trafficmonitor.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_mcou_antispam.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_offguard.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_pdm.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_popupchk.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_procmon.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_profiles.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_qb.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_scaner.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_scheduler.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_settings.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_spamtest.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_startupenum2.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_sys.h
# End Source File
# Begin Source File

SOURCE=..\include\structs\s_taskmanager.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_updater.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_updater2005.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_updater2005settings.h
# End Source File
# Begin Source File

SOURCE=..\Include\structs\s_urlflt.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AlertDialog.h
# End Source File
# Begin Source File

SOURCE=.\avpgui.h
# End Source File
# Begin Source File

SOURCE=.\FWRules.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\ListsView.h
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.\NewsReader.h
# End Source File
# Begin Source File

SOURCE=.\pid_gui_fix.h
# End Source File
# Begin Source File

SOURCE=.\ProfilesView.h
# End Source File
# Begin Source File

SOURCE=.\ReportWindow.h
# End Source File
# Begin Source File

SOURCE=.\RootSink.h
# End Source File
# Begin Source File

SOURCE=.\ScanObjects.h
# End Source File
# Begin Source File

SOURCE=.\ServiceSettings.h
# End Source File
# Begin Source File

SOURCE=.\ServicesView.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlgs.h
# End Source File
# Begin Source File

SOURCE=.\SettingsWindow.h
# End Source File
# Begin Source File

SOURCE=..\Upgrade\ss.h
# End Source File
# Begin Source File

SOURCE=.\StatusWindow.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskProfile.h
# End Source File
# Begin Source File

SOURCE=.\TaskProfileProduct.h
# End Source File
# Begin Source File

SOURCE=.\TaskView.h
# End Source File
# Begin Source File

SOURCE=.\ThreatsView.h
# End Source File
# Begin Source File

SOURCE=.\tmclient.h
# End Source File
# Begin Source File

SOURCE=.\TrayIcon.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=.\Wizards.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avpgui.imp
# End Source File
# Begin Source File

SOURCE=.\avpgui.meta
# End Source File
# Begin Source File

SOURCE=.\avpgui.rc
# End Source File
# Begin Source File

SOURCE=.\avpgui.rgs
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Skin Files"

# PROP Default_Filter ""
# Begin Group "Icons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\images\adapter.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\bootsect.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\error.ico

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\skin\images\help.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\info.ico

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\skin\images\internet.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\key.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\local.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\Mail.ico

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\skin\images\main_off.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\main_on.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\memory.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\navstate.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\nempty.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\nonrecursive.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\ok.ico

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\skin\images\pause.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\red.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\run.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\startupobj.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_mail.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_off.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_on.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_red.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_scan.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_script.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_web.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\tray\tray_yellow.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\trusted.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\unkobj.ico
# End Source File
# Begin Source File

SOURCE=.\skin\images\warning.ico

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\skin\images\yellow.ico
# End Source File
# End Group
# Begin Group "Bitmaps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\images\help.bmp
# End Source File
# Begin Source File

SOURCE=.\skin\images\ie.bmp
# End Source File
# Begin Source File

SOURCE=.\skin\images\network.bmp
# End Source File
# Begin Source File

SOURCE=.\skin\images\protection.bmp
# End Source File
# Begin Source File

SOURCE=.\skin\images\settings.bmp
# End Source File
# Begin Source File

SOURCE=.\skin\images\taskbar.bmp
# End Source File
# Begin Source File

SOURCE=.\skin\images\title.bmp
# End Source File
# End Group
# Begin Group "Ini"

# PROP Default_Filter ""
# Begin Group "layout"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\layout\ah.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\as.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\bb.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\cf.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\main.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\mc.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\oas.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\prot.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\report.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\sc.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\scan.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\service.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\settings.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\spy.ini
# End Source File
# Begin Source File

SOURCE=.\skin\layout\updater.ini
# End Source File
# End Group
# Begin Group "localize"

# PROP Default_Filter ""
# Begin Group "en"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\en\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\credits.loc
# End Source File
# Begin Source File

SOURCE=".\skin\en\iso3166-1.loc"
# End Source File
# Begin Source File

SOURCE=.\skin\en\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\en\updater.loc
# End Source File
# End Group
# Begin Group "ru"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\ru\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\credits.loc
# End Source File
# Begin Source File

SOURCE=".\skin\ru\iso3166-1.loc"
# End Source File
# Begin Source File

SOURCE=.\skin\ru\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\ru\updater.loc
# End Source File
# End Group
# Begin Group "de"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\DE\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\credits.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\hints.loc
# End Source File
# Begin Source File

SOURCE=".\skin\DE\iso3166-1.loc"
# End Source File
# Begin Source File

SOURCE=.\skin\DE\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\DE\updater.loc
# End Source File
# End Group
# Begin Group "fr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\FR\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\credits.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\hints.loc
# End Source File
# Begin Source File

SOURCE=".\skin\FR\iso3166-1.loc"
# End Source File
# Begin Source File

SOURCE=.\skin\FR\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\FR\updater.loc
# End Source File
# End Group
# Begin Group "esp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\esp\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\credits.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\hints.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\esp\updater.loc
# End Source File
# End Group
# Begin Group "nl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\NL\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\credits.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\hints.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\NL\updater.loc
# End Source File
# End Group
# Begin Group "it"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\skin\it\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\as.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\cf.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\credits.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\hints.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\main.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\mc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\oas.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\prot.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\report.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\sc.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\scan.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\service.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\settings.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\spy.loc
# End Source File
# Begin Source File

SOURCE=.\skin\it\updater.loc
# End Source File
# End Group
# Begin Source File

SOURCE=.\skin\ah.loc
# End Source File
# Begin Source File

SOURCE=.\skin\bb.loc
# End Source File
# Begin Source File

SOURCE=.\skin\enums.loc
# End Source File
# Begin Source File

SOURCE=.\skin\prot.loc
# End Source File
# End Group
# Begin Source File

SOURCE=.\skin\skin.ini

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
ProjDir=.
TargetDir=\Projects\KAVP7\out\Debug
InputPath=.\skin\skin.ini

"$(TargetDir)\skin\skin.ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy "$(ProjDir)\skin" "$(TargetDir)\skin" /Y /R /S /I

# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\avpgui.dep
# End Source File
# Begin Source File

SOURCE=.\avpgui.dsc
# End Source File
# Begin Source File

SOURCE=.\avpgui.mak
# End Source File
# Begin Source File

SOURCE=.\guitest.htm
# End Source File
# End Target
# End Project

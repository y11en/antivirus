# Microsoft Developer Studio Project File - Name="AdminGUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AdminGUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AdminGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AdminGUI.mak" CFG="AdminGUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AdminGUI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AdminGUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/AdminGUI", IRCSAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AdminGUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Out\Release"
# PROP Intermediate_Dir "..\..\Temp\Release\ppp\AdminGUI"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADMINGUI_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\CSShare" /I "..\..\CommonFiles" /I "..\Include" /I "..\..\Prague\Include" /I "..\..\Updater60\include" /I "..\Basegui" /I "..\GUI60" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADMINGUI_EXPORTS" /D "_PRAGUE_TRACE_" /D "ADMINGUI_WKS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib klcsstd.lib /nologo /dll /debug /machine:I386 /out:"..\..\Out\Release/ap_wks.dll" /libpath:"lib" /libpath:"..\..\Out\Release"

!ELSEIF  "$(CFG)" == "AdminGUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\Temp\Debug\ppp\AdminGUI"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADMINGUI_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\CSShare" /I "..\..\CommonFiles" /I "..\Include" /I "..\..\Prague\Include" /I "..\..\Updater60\include" /I "..\Basegui" /I "..\GUI60" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ADMINGUI_EXPORTS" /D "_PRAGUE_TRACE_" /D "ADMINGUI_WKS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib klcsstd.lib /nologo /dll /debug /machine:I386 /out:"..\..\Out\Debug/ap_wks.dll" /pdbtype:sept /libpath:"lib" /libpath:"..\..\Out\Debug"

!ENDIF 

# Begin Target

# Name "AdminGUI - Win32 Release"
# Name "AdminGUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "COM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\admingui.idl

!IF  "$(CFG)" == "AdminGUI - Win32 Release"

# ADD MTL /tlb "..\..\Out\Release/ap_wks.tlb" /h "comgui.h" /iid "admingui_i.c"
# SUBTRACT MTL /mktyplib203

!ELSEIF  "$(CFG)" == "AdminGUI - Win32 Debug"

# ADD MTL /tlb "..\..\Out\Debug/ap_wks.tlb" /h "comgui.h" /iid "admingui_i.c"
# SUBTRACT MTL /mktyplib203

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\basegui.idl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ComAdminGui.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ComAdminGui.h
# End Source File
# Begin Source File

SOURCE=.\ComBaseGui.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\AdminGUI.cpp
# End Source File
# Begin Source File

SOURCE=.\admingui.def
# End Source File
# Begin Source File

SOURCE=..\Include\common\Converter.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CSIfacesImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\GUI60\FWRules.cpp
# End Source File
# Begin Source File

SOURCE=..\gui60\ListsView.cpp
# End Source File
# Begin Source File

SOURCE=.\PragueLoader.cpp
# End Source File
# Begin Source File

SOURCE=..\gui60\RootSink.cpp
# End Source File
# Begin Source File

SOURCE=..\gui60\ScanObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\gui60\ServiceSettings.cpp
# End Source File
# Begin Source File

SOURCE=..\GUI60\SettingsDlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlgsAdm.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsPages.cpp
# End Source File
# Begin Source File

SOURCE=..\GUI60\SettingsWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\GUI60\TaskProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProfileAdm.cpp
# End Source File
# Begin Source File

SOURCE=..\GUI60\TaskView.cpp
# End Source File
# Begin Source File

SOURCE=..\Include\common\Tracer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AdminGUI.h
# End Source File
# Begin Source File

SOURCE=..\Connector\common.h
# End Source File
# Begin Source File

SOURCE=..\Include\common\Converter.h
# End Source File
# Begin Source File

SOURCE=.\CSIfacesImpl.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\FWRules.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\ListsView.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\pid_gui_fix.h
# End Source File
# Begin Source File

SOURCE=.\PragueLoader.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\RootSink.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\ScanObjects.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\SettingsDlgs.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlgsAdm.h
# End Source File
# Begin Source File

SOURCE=.\SettingsPages.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\SettingsWindow.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\TaskProfile.h
# End Source File
# Begin Source File

SOURCE=.\TaskProfileAdm.h
# End Source File
# Begin Source File

SOURCE=..\GUI60\TaskView.h
# End Source File
# Begin Source File

SOURCE=..\Include\common\Tracer.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AdminGUI.rc
# End Source File
# Begin Source File

SOURCE=.\admingui.rgs
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE="..\Install AdmPlg\admgui.reg"
# End Source File
# Begin Source File

SOURCE=..\Install\ppp.reg
# End Source File
# End Target
# End Project

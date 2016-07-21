# Microsoft Developer Studio Project File - Name="COMMON" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=COMMON - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "common.mak" CFG="COMMON - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "COMMON - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "COMMON - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/Kaspersky Anti-Virus Personal 5.0 MP1/CommonFiles/common", FUFKAAAA"
# PROP Scc_LocalPath "..\projects"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "COMMON - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\temp\Release\COMMON"
# PROP Intermediate_Dir "..\..\temp\Release\COMMON"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "BUILD_COMMON_DLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Version.lib htmlhelp.lib sign.lib winmm.lib Win32Utils.lib /nologo /base:"@..\..\dll_bases.txt,COMMON" /subsystem:windows /dll /pdb:"..\..\Out\Release/klcmn.pdb" /debug /machine:I386 /out:"..\..\Out\Release/klcmn.dll" /pdbtype:sept /libpath:"..\ReleaseDll\\"
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\Release\Kaspersky Anti-Virus Personal 5.0\Out\Release\klcmn.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "COMMON - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\temp\Debug\COMMON"
# PROP Intermediate_Dir "..\..\temp\Debug\COMMON"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "BUILD_COMMON_DLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Win32Utils.lib Version.lib htmlhelp.lib sign.lib winmm.lib Msimg32.lib /nologo /base:"@..\..\dll_bases.txt,COMMON" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\Out\Debug/klcmn.dll" /libpath:"..\DebugDll\\"
# SUBTRACT LINK32 /profile /map

!ENDIF 

# Begin Target

# Name "COMMON - Win32 Release"
# Name "COMMON - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BaseListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\BasePropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseObjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\COMMON.cpp
# End Source File
# Begin Source File

SOURCE=.\COMMON.def
# End Source File
# Begin Source File

SOURCE=.\COMMON.rc
# End Source File
# Begin Source File

SOURCE=..\StuffIO\CompressPath.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CRestrictEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpandButton.cpp
# End Source File
# Begin Source File

SOURCE=.\FileDialogImpl.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\filedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GrayListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Layout.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\LockedBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\MailWeb.cpp
# End Source File
# Begin Source File

SOURCE=.\MsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureEx.cpp
# End Source File
# Begin Source File

SOURCE=.\PNGImage.cpp
# End Source File
# Begin Source File

SOURCE=.\popup.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportCtrlHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableLayout.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizablePage.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizableSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\RichText.cpp
# End Source File
# Begin Source File

SOURCE=.\RichTextImpl.cpp
# End Source File
# Begin Source File

SOURCE=.\Separator.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TrackToolTip.cpp
# End Source File
# Begin Source File

SOURCE=.\TransparentIco.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\WhiteBarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XPThemeHelper.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoPtr.h
# End Source File
# Begin Source File

SOURCE=.\BaseListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\BasePropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\ChooseObjectDialog.h
# End Source File
# Begin Source File

SOURCE=.\COMMON.h
# End Source File
# Begin Source File

SOURCE=.\CRestrictEdit.h
# End Source File
# Begin Source File

SOURCE=.\DEFS.H
# End Source File
# Begin Source File

SOURCE=.\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\DragWnd.h
# End Source File
# Begin Source File

SOURCE=.\ExpandButton.h
# End Source File
# Begin Source File

SOURCE=.\FileDialogImpl.h
# End Source File
# Begin Source File

SOURCE=.\filedlg.h
# End Source File
# Begin Source File

SOURCE=.\Focuser.h
# End Source File
# Begin Source File

SOURCE=.\FontDefs.h
# End Source File
# Begin Source File

SOURCE=.\GrayListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HyperStatic.h
# End Source File
# Begin Source File

SOURCE=.\InfoPanel.h
# End Source File
# Begin Source File

SOURCE=.\Layout.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LockedBitmap.h
# End Source File
# Begin Source File

SOURCE=.\MailWeb.h
# End Source File
# Begin Source File

SOURCE=.\MapiWrapper.h
# End Source File
# Begin Source File

SOURCE=.\MsgBox.h
# End Source File
# Begin Source File

SOURCE=.\MsgBoxInternal.h
# End Source File
# Begin Source File

SOURCE=.\PictureEx.h
# End Source File
# Begin Source File

SOURCE=.\PNGImage.h
# End Source File
# Begin Source File

SOURCE=.\popup.h
# End Source File
# Begin Source File

SOURCE=.\ReportCtrlHeader.h
# End Source File
# Begin Source File

SOURCE=.\ReportListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ResizableLayout.h
# End Source File
# Begin Source File

SOURCE=.\ResizablePage.h
# End Source File
# Begin Source File

SOURCE=.\ResizableSheet.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ResourceHelper.h
# End Source File
# Begin Source File

SOURCE=.\RichText.h
# End Source File
# Begin Source File

SOURCE=.\RichTextImpl.h
# End Source File
# Begin Source File

SOURCE=.\Separator.h
# End Source File
# Begin Source File

SOURCE=.\ShellHelper.h
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TrackToolTip.h
# End Source File
# Begin Source File

SOURCE=.\TransparentIco.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\WhiteBarDlg.h
# End Source File
# Begin Source File

SOURCE=.\WinVerDiff.h
# End Source File
# Begin Source File

SOURCE=.\XPThemeHelper.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\common.rc2
# End Source File
# Begin Source File

SOURCE=.\RES\error.ico
# End Source File
# Begin Source File

SOURCE=.\RES\fewerinfo.ico
# End Source File
# Begin Source File

SOURCE=.\RES\help.ico
# End Source File
# Begin Source File

SOURCE=.\RES\inf.ico
# End Source File
# Begin Source File

SOURCE=.\RES\LINKHAND.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\lock.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\lock_m.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\lockdot.ico
# End Source File
# Begin Source File

SOURCE=.\RES\moreinfo.ico
# End Source File
# Begin Source File

SOURCE=.\RES\ok.ico
# End Source File
# Begin Source File

SOURCE=.\RES\sortdown.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\sortup.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\warn.ico
# End Source File
# End Group
# Begin Group "CImage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CIMAGE\cimage.h
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\cimageb.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\cimageb.h
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\cmap.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\cmap.h
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\Dibutils.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\Dibutils.h
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\Imapng.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\CIMAGE\Imapng.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RES\error24x24.png
# End Source File
# Begin Source File

SOURCE=.\RES\error32x32.png
# End Source File
# Begin Source File

SOURCE=.\RES\info24x24.png
# End Source File
# Begin Source File

SOURCE=.\RES\info32x32.png
# End Source File
# Begin Source File

SOURCE=.\RES\ok24x24.png
# End Source File
# Begin Source File

SOURCE=.\RES\ok32x32.png
# End Source File
# Begin Source File

SOURCE=.\RES\pause24x24.png
# End Source File
# Begin Source File

SOURCE=.\RES\pause32x32.png
# End Source File
# Begin Source File

SOURCE=.\RES\warning24x24.png
# End Source File
# Begin Source File

SOURCE=.\RES\warning32x32.png
# End Source File
# End Target
# End Project

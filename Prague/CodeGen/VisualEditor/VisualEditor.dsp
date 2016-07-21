# Microsoft Developer Studio Project File - Name="VisualEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=VisualEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VisualEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VisualEditor.mak" CFG="VisualEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VisualEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "VisualEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/CodeGen/VisualEditor", VAYAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VisualEditor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/CodeGen/VisualEditor"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/CodeGen/VisualEditor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GR /GX /Zi /O2 /I "..\..\..\Windows" /I "..\..\..\CommonFiles" /I "..\..\Include" /I "..\..\..\property" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /i "..\..\..\Windows" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imagehlp.lib DTUtils.lib AVPControls.lib SWM.lib KLDTSer.lib WinAvpIO.lib Property.lib Win32Utils.lib mpr.lib hashutil.lib ifaceinfo.lib /subsystem:windows /debug /machine:I386 /libpath:"../../../out/release" /libpath:"../../../CommonFiles/releasedll"
# SUBTRACT LINK32 /nologo

!ELSEIF  "$(CFG)" == "VisualEditor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/CodeGen/VisualEditor"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/CodeGen/VisualEditor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\Windows" /I "..\..\..\CommonFiles" /I "..\..\Include" /I "..\..\..\property" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x417 /i "..\..\..\Windows" /i "..\..\..\CommonFiles" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imagehlp.lib DTUtils.lib AVPControls.lib KLDTSer.lib SWM.lib WinAvpIO.lib Property.lib mpr.lib Win32Utils.lib hashutil.lib ifaceinfo.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../../../out/debug" /libpath:"../../../CommonFiles/debugdll"

!ENDIF 

# Begin Target

# Name "VisualEditor - Win32 Release"
# Name "VisualEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChooseNameFromListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGenTools.cpp
# End Source File
# Begin Source File

SOURCE=.\CodeGenWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\CodePages.cpp
# End Source File
# Begin Source File

SOURCE=.\controltreectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayString.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneralOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneratorOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GeneratorSummaryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HeaderGenWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportDefFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LargeCommentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MethDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewInterfaceNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\PPOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\PrepareDBTree.cpp
# End Source File
# Begin Source File

SOURCE=.\PropMemberDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PVE_DT_UTILS.cpp
# End Source File
# Begin Source File

SOURCE=.\RenameIntRelatedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectInterfacesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectLoadedInterfacesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\splitterctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SynchroTools.cpp
# End Source File
# Begin Source File

SOURCE=.\VEControlTree.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditor.rc

!IF  "$(CFG)" == "VisualEditor - Win32 Release"

# ADD BASE RSC /l 0x419
# ADD RSC /l 0x419 /i "..\..\..\Windows\avpcontrols"
# SUBTRACT RSC /x

!ELSEIF  "$(CFG)" == "VisualEditor - Win32 Debug"

# ADD BASE RSC /l 0x419
# ADD RSC /l 0x419 /i "..\..\..\Windows\AVPControls"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VisualEditorClipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorCreateIT.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorDlgTree.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorEnable.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorLoadIT.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorLoadSave.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorRecent.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorSaveDB.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorSrcGen.cpp
# End Source File
# Begin Source File

SOURCE=.\VisualEditorUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\StuffMFC\XStatusBar.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\AVPPveID.h
# End Source File
# Begin Source File

SOURCE=.\ChooseNameFromListDlg.h
# End Source File
# Begin Source File

SOURCE=.\CodeGenTools.h
# End Source File
# Begin Source File

SOURCE=.\CodeGenWizard.h
# End Source File
# Begin Source File

SOURCE=.\CodePages.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\COMUtils.h
# End Source File
# Begin Source File

SOURCE=.\controltreectrl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\FAutomat.h
# End Source File
# Begin Source File

SOURCE=.\GeneralOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\GeneratorOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\GeneratorOptionsFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\GeneratorSummaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\HeaderGenWizard.h
# End Source File
# Begin Source File

SOURCE=..\IDServerInterface.h
# End Source File
# Begin Source File

SOURCE=.\ImportDefFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\LargeCommentDlg.h
# End Source File
# Begin Source File

SOURCE=.\Layout.h
# End Source File
# Begin Source File

SOURCE=.\MethDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewInterfaceNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=..\CodeGen\PCGDescrId.h
# End Source File
# Begin Source File

SOURCE=..\CodeGen\PCGInterface.h
# End Source File
# Begin Source File

SOURCE=.\PPOptions.h
# End Source File
# Begin Source File

SOURCE=.\PrepareDBTree.h
# End Source File
# Begin Source File

SOURCE=.\PropMemberDlg.h
# End Source File
# Begin Source File

SOURCE=.\PVE_DT_UTILS.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Stuff\RegStorage.h
# End Source File
# Begin Source File

SOURCE=.\RenameIntRelatedDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectGeneratorDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectInterfacesDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectLoadedInterfacesDlg.h
# End Source File
# Begin Source File

SOURCE=.\splitterctrl.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SynchroTools.h
# End Source File
# Begin Source File

SOURCE=.\VEControlTree.h
# End Source File
# Begin Source File

SOURCE=.\VisualEditor.h
# End Source File
# Begin Source File

SOURCE=.\VisualEditorDlg.h
# End Source File
# Begin Source File

SOURCE=.\VisualEditorDlg.inl
# End Source File
# Begin Source File

SOURCE=.\Win95adg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\InterfaceD.ico
# End Source File
# Begin Source File

SOURCE=.\res\InterfaceE.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\VisualEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\VisualEditor.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section VisualEditor : {D2B34AE6-B9CF-11D2-96B0-00104B5B66AA}
# 	2:5:Class:CControlTreeCtrl
# 	2:10:HeaderFile:controltreectrl.h
# 	2:8:ImplFile:controltreectrl.cpp
# End Section
# Section VisualEditor : {D4828028-8152-11D3-96B0-00104B5B66AA}
# 	2:5:Class:CSplitterCtrl
# 	2:10:HeaderFile:splitterctrl.h
# 	2:8:ImplFile:splitterctrl.cpp
# End Section
# Section VisualEditor : {D482802A-8152-11D3-96B0-00104B5B66AA}
# 	2:21:DefaultSinkHeaderFile:splitterctrl.h
# 	2:16:DefaultSinkClass:CSplitterCtrl
# End Section
# Section VisualEditor : {D2B34AE8-B9CF-11D2-96B0-00104B5B66AA}
# 	2:21:DefaultSinkHeaderFile:controltreectrl.h
# 	2:16:DefaultSinkClass:CControlTreeCtrl
# End Section

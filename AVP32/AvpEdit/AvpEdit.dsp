# Microsoft Developer Studio Project File - Name="AvpEdit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AvpEdit - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AvpEdit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvpEdit.mak" CFG="AvpEdit - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvpEdit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AvpEdit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/AvpEdit", IAEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AvpEdit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "." /I "..\\" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\..\commonfiles" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 sign.lib winmm.lib mpr.lib imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../out/release/AvpEdit.exe" /libpath:"..\..\out\release" /MAPINFO:LINES /IGNORE:4098
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\release\AvpEdit.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AvpEdit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\..\commonfiles" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 sign.lib winmm.lib mpr.lib imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../out/debug/AvpEdit.exe" /libpath:"../../out/debug" /IGNORE:4098
# SUBTRACT LINK32 /pdb:none /incremental:no /nodefaultlib
# Begin Special Build Tool
TargetPath=\out\debug\AvpEdit.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AvpEdit - Win32 Release"
# Name "AvpEdit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AvpEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\AvpEditDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\AvpEditView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DDX_Hex.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Getdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Layout.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Opensgn.cpp
# End Source File
# Begin Source File

SOURCE=.\OptDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\REdit.cpp
# End Source File
# Begin Source File

SOURCE=..\Avp_32\Scandir.cpp
# End Source File
# Begin Source File

SOURCE=.\ScanDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Sq_s.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Sq_u.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TEdit.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\AVE_SQL\AVE_Sql.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avp_dll.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avp_msg.h
# End Source File
# Begin Source File

SOURCE=.\AvpEdit.h
# End Source File
# Begin Source File

SOURCE=.\AvpEditDoc.h
# End Source File
# Begin Source File

SOURCE=.\AvpEditView.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\Dbf_defs.h
# End Source File
# Begin Source File

SOURCE=.\Dbf_head.h
# End Source File
# Begin Source File

SOURCE=.\Dbf_recd.h
# End Source File
# Begin Source File

SOURCE=.\DDX_Hex.h
# End Source File
# Begin Source File

SOURCE=.\dialogs.h
# End Source File
# Begin Source File

SOURCE=.\FindDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\Key.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Bases\Method.h
# End Source File
# Begin Source File

SOURCE=..\Obj16.h
# End Source File
# Begin Source File

SOURCE=.\OptDlg.h
# End Source File
# Begin Source File

SOURCE=.\REdit.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\RetFlags.h
# End Source File
# Begin Source File

SOURCE=..\AVP_32\Scandir.h
# End Source File
# Begin Source File

SOURCE=.\ScanDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Scanobj.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Bases\Sizes.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TEdit.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avpedit.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AvpEdit.ico
# End Source File
# Begin Source File

SOURCE=.\AvpEdit.rc
# End Source File
# Begin Source File

SOURCE=.\res\AvpEdit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\AvpEditDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\kl89x21.bmp
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project

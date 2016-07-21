# Microsoft Developer Studio Project File - Name="Avp32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Avp32 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avp32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp32.mak" CFG="Avp32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Avp32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/AVP_32", YJEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avp32 - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\..\commonfiles" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib mpr.lib imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../out/release/Avp32.exe" /libpath:"..\release" /stub:$(IntDir)\avp_stub.exe /MAPINFO:LINES /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\release\Avp32.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

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
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /i "..\..\commonfiles" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib mpr.lib imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../out/debug/Avp32.exe" /libpath:"..\debug" /stub:$(IntDir)\avp_stub.exe /IGNORE:4098
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\debug\Avp32.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Avp32 - Win32 Release"
# Name "Avp32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\aboutviewctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Agentint.cpp
# End Source File
# Begin Source File

SOURCE=.\Avp32.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\Avp32.hpj
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Avp32Dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\CheckSelfExp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\CheckUse.cpp
# End Source File
# Begin Source File

SOURCE=.\cust.cpp
# End Source File
# Begin Source File

SOURCE=.\DisDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProxy.cpp
# End Source File
# Begin Source File

SOURCE=.\FindDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\StuffMFC\Globals.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Layout.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\LoadCCClientDLL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\LoadKeys.cpp
# End Source File
# Begin Source File

SOURCE=.\LogCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\StuffMFC\NewToolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\Policy.cpp
# End Source File
# Begin Source File

SOURCE=.\reportviewctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Scandir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Sq_u.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabPages.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=.\aboutviewctrl.h
# End Source File
# Begin Source File

SOURCE=.\Agentint.h
# End Source File
# Begin Source File

SOURCE=..\AVE_SQL\AVE_Fld.h
# End Source File
# Begin Source File

SOURCE=..\AVE_SQL\AVE_Sql.h
# End Source File
# Begin Source File

SOURCE=.\Avp32.h
# End Source File
# Begin Source File

SOURCE=.\Avp32Dlg.h
# End Source File
# Begin Source File

SOURCE=..\Avp_aler.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avp_dll.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avp_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\AVPRegID.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\CheckUse.h
# End Source File
# Begin Source File

SOURCE=.\cust.h
# End Source File
# Begin Source File

SOURCE=.\DisDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgProxy.h
# End Source File
# Begin Source File

SOURCE=.\FindDlg.h
# End Source File
# Begin Source File

SOURCE=..\fn.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\StuffMFC\Globals.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\Key.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Layout.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\LoadCCClientDLL.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\LoadKeys.h
# End Source File
# Begin Source File

SOURCE=.\LogCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\StuffMFC\newtoolbar.h
# End Source File
# Begin Source File

SOURCE=.\Pid.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\Policy.h
# End Source File
# Begin Source File

SOURCE=.\reportviewctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\RetFlags.h
# End Source File
# Begin Source File

SOURCE=.\Scandir.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Scanobj.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabPages.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Win95adg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\1.ico
# End Source File
# Begin Source File

SOURCE=.\res\10.ico
# End Source File
# Begin Source File

SOURCE=.\res\12.ico
# End Source File
# Begin Source File

SOURCE=.\res\2.ico
# End Source File
# Begin Source File

SOURCE=.\res\3.ico
# End Source File
# Begin Source File

SOURCE=.\res\4.ico
# End Source File
# Begin Source File

SOURCE=.\res\5.ico
# End Source File
# Begin Source File

SOURCE=.\res\6.ico
# End Source File
# Begin Source File

SOURCE=.\res\8.ico
# End Source File
# Begin Source File

SOURCE=.\res\9.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\avp1.bmp
# End Source File
# Begin Source File

SOURCE=.\Avp32.rc
# End Source File
# Begin Source File

SOURCE=.\res\Avp32.rc2
# End Source File
# Begin Source File

SOURCE=.\res\binboot.bin
# End Source File
# Begin Source File

SOURCE=.\res\binmbr.bin
# End Source File
# Begin Source File

SOURCE=.\res\icon12.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon14.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon__9.ico
# End Source File
# Begin Source File

SOURCE=.\res\iconover.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\Avi\Infect.avi
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\kaspersky1.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\KAV1.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\kl89x21.bmp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\klsmall.bmp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\LogoBe2.bmp
# End Source File
# Begin Source File

SOURCE=.\Avi\Ok.avi
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Avi\Search.avi
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\tb_rep.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Avp32.dep
# End Source File
# Begin Source File

SOURCE=.\Avp32.mak
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ReleaseDll\Sign.lib
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ReleaseDll\SWM.lib
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ReleaseDll\Property.lib
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ReleaseDll\KLDTSer.lib
# End Source File
# End Target
# End Project
# Section Avp32 : {8C397FA6-6CE7-11D3-96B0-00104B5B66AA}
# 	2:5:Class:CAboutViewCtrl
# 	2:10:HeaderFile:aboutviewctrl.h
# 	2:8:ImplFile:aboutviewctrl.cpp
# End Section
# Section Avp32 : {DD54D567-2FCA-11D3-96B0-00104B5B66AA}
# 	2:21:DefaultSinkHeaderFile:reportviewctrl.h
# 	2:16:DefaultSinkClass:CReportViewCtrl
# End Section
# Section Avp32 : {8C397FA8-6CE7-11D3-96B0-00104B5B66AA}
# 	2:21:DefaultSinkHeaderFile:aboutviewctrl.h
# 	2:16:DefaultSinkClass:CAboutViewCtrl
# End Section
# Section Avp32 : {DD54D565-2FCA-11D3-96B0-00104B5B66AA}
# 	2:5:Class:CReportViewCtrl
# 	2:10:HeaderFile:reportviewctrl.h
# 	2:8:ImplFile:reportviewctrl.cpp
# End Section

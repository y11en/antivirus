# Microsoft Developer Studio Project File - Name="Avp95 VxD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Avp95 VxD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avp95 VxD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp95 VxD.mak" CFG="Avp95 VxD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp95 VxD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Avp95 VxD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/Avp95", QYDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avp95 VxD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /Zi /I "..\\" /I "..\..\CommonFiles" /I "..\vtoolsd\include" /I "$(VTOOLSD)\include" /D "VTOOLSD" /D "NDEBUG" /D "_X86_" /D "IS_32" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Yu"stdafx.h" /Zl /FD /Gs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib /nologo /debug /machine:IX86 /nodefaultlib /def:".\avp95.def" /out:"../../out/release/AVP95.vxd" /libpath:"$(VTOOLSD)\lib" /libpath:"..\..\commonfiles\release" /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA /IGNORE:4078,4039,4070
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\release\AVP95.vxd
TargetName=AVP95
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd RELEASE $(TargetPath) $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Avp95 VxD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /Zp1 /MTd /W3 /Gm /Zi /Od /I "..\\" /I "..\..\CommonFiles" /I "..\vtoolsd\include" /I "$(VTOOLSD)\include" /D "VTOOLSD" /D "DEBUG" /D "_DEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Yu"stdafx.h" /FD /Gs /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icrtms.obj clms4d.lib daamsd.lib ndms95d.lib rtmsd.lib ntmsd.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib chkesp.obj /nologo /debug /machine:IX86 /nodefaultlib /def:".\avp95.def" /out:"../../out/debug/AVP95.vxd" /libpath:"$(VTOOLSD)\lib" /libpath:"..\..\commonfiles\release" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\debug\AVP95.vxd
TargetName=AVP95
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd DEBUG $(TargetPath) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Avp95 VxD - Win32 Release"
# Name "Avp95 VxD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Avp95.cpp
# End Source File
# Begin Source File

SOURCE=.\avp95.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Avp95.rc
# End Source File
# Begin Source File

SOURCE=.\Avp95api.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\avpfpi.cpp
# End Source File
# Begin Source File

SOURCE=.\avpsign.cpp
# End Source File
# Begin Source File

SOURCE=..\DBCleanUp\DBCleanUp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\DBCleanUp\DirIterators9x.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\File_io.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\IOCache\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\vxdio.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\_carray.h
# End Source File
# Begin Source File

SOURCE=.\Avp95.h
# End Source File
# Begin Source File

SOURCE=.\Avp95api.h
# End Source File
# Begin Source File

SOURCE=.\Avp95ctl.h
# End Source File
# Begin Source File

SOURCE=..\FPI\avp_cs.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avp_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avpioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\BaseAPI.h
# End Source File
# Begin Source File

SOURCE=..\DBCleanUp\DBCleanUp.h
# End Source File
# Begin Source File

SOURCE=.\FILE_IO.H
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\KLDTSer.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\RetFlags.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Scanobj.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Avp95.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\PostBuild.cmd
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Release\Property.lib
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Release\KLDTSer.lib
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Release\Sign.lib
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Release\SWM.lib
# End Source File
# End Target
# End Project

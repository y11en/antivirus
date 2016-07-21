# Microsoft Developer Studio Project File - Name="KLE_VXD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=KLE_VXD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KLE_VXD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KLE_VXD.mak" CFG="KLE_VXD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KLE_VXD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "KLE_VXD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/KLE_VXD", QYDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KLE_VXD - Win32 Release"

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
# ADD CPP /nologo /G3 /Zp1 /MT /W3 /O2 /Oy- /I "..\\" /I "$(VTOOLSD)\include" /D "NDEBUG" /D "_X86_" /D "IS_32" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Yu"stdafx.h" /Zl /FD /Gs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib /nologo /pdb:none /map:"..\Release/KLE_VXD.map" /machine:IX86 /nodefaultlib /def:".\KLE_VXD.def" /out:"..\Release/KLE_VXD.vxd" /libpath:"$(VTOOLSD)\lib" /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA /IGNORE:4078,4039,4070
# Begin Special Build Tool
TargetPath=\Work\avp32\Release\KLE_VXD.vxd
TargetName=KLE_VXD
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=$(MSVC16)\bin\rc -i "..\..\Common~1" -r $(TargetName).rc	$(VTOOLSD)\bin\sethdr -n $(TargetName) -x $(TargetPath) -r $(TargetName).res	tsigner $(TargetPath)	sync
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KLE_VXD - Win32 Debug"

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
# ADD CPP /nologo /G3 /Zp1 /MTd /W3 /Gm /Zi /Od /I "..\\" /I "$(VTOOLSD)\include" /D "DEBUG" /D "_DEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Yu"stdafx.h" /FD /Gs /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icrtms.obj clms4d.lib daamsd.lib ndms95d.lib rtmsd.lib ntmsd.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib chkesp.obj /nologo /debug /machine:IX86 /nodefaultlib /def:".\KLE_VXD.def" /out:"..\Debug/KLE_VXD.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Work\avp32\Debug\KLE_VXD.vxd
TargetName=KLE_VXD
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=$(MSVC16)\bin\rc -i "..\..\Common~1" -r $(TargetName).rc	$(VTOOLSD)\bin\sethdr -n $(TargetName) -x $(TargetPath) -r $(TargetName).res	tsigner $(TargetPath)	$(SOFTICE)\nmsym /TRANSLATE /LOAD $(TargetPath)	sync
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "KLE_VXD - Win32 Release"
# Name "KLE_VXD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\File_io.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\IOCache\FileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\KLE\KLE_IPL.cpp
# End Source File
# Begin Source File

SOURCE=.\KLE_VXD.cpp
# End Source File
# Begin Source File

SOURCE=.\KLE_VXD.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\KLE_VXD.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\vxdio.cpp
# End Source File
# Begin Source File

SOURCE=.\vxdsign.cpp
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

SOURCE=..\..\CommonFiles\ScanAPI\Avp_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\Avpioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\ScanAPI\BaseAPI.h
# End Source File
# Begin Source File

SOURCE=.\FILE_IO.H
# End Source File
# Begin Source File

SOURCE=..\KLE\KLE_IPL.h
# End Source File
# Begin Source File

SOURCE=.\KLE_VXD.h
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

SOURCE=.\KLE_VXD.rc2
# End Source File
# End Group
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

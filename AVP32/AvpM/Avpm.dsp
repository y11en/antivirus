# Microsoft Developer Studio Project File - Name="Avp Monitor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Avp Monitor - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avpm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avpm.mak" CFG="Avp Monitor - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp Monitor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Avp Monitor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/Avpm", TBEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avp Monitor - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "." /I "..\\" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 msvcrt.lib oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib /out:"../../out/release/Avpm.exe" /MAPINFO:LINES
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\release\Avpm.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Avp Monitor - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\CommonFiles" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 msvcrtd.lib oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib winmm.lib imagehlp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib
# SUBTRACT LINK32 /incremental:no /map
# Begin Special Build Tool
TargetPath=\avp32\Debug\Avpm.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Avp Monitor - Win32 Release"
# Name "Avp Monitor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\_carray.cpp
# End Source File
# Begin Source File

SOURCE=.\Agentint.cpp
# End Source File
# Begin Source File

SOURCE=.\avpm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\CheckSelfExp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\CheckUse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\LoadCCClientDLL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\LoadKeys.cpp

!IF  "$(CFG)" == "Avp Monitor - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "Avp Monitor - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\Policy.cpp
# End Source File
# Begin Source File

SOURCE=.\Setopt.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Sq_u.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\_carray.h
# End Source File
# Begin Source File

SOURCE=.\Agentint.h
# End Source File
# Begin Source File

SOURCE=..\Avp95\Avp95ctl.h
# End Source File
# Begin Source File

SOURCE=..\Avp_aler.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\CalcSum.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\CheckUse.h
# End Source File
# Begin Source File

SOURCE=..\fn.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\Key.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\LoadCCClientDLL.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\LoadKeys\LoadKeys.h
# End Source File
# Begin Source File

SOURCE=.\Pid.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\CCClient\Policy.h
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# Begin Source File

SOURCE=.\Vxd_msg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\avp.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\avp1.bmp
# End Source File
# Begin Source File

SOURCE=.\Avpm.rc
# End Source File
# Begin Source File

SOURCE=.\Avpm.rc2
# End Source File
# Begin Source File

SOURCE=.\idr_main.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\KAV1.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\KAV_dis.ico
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Res\Kl89x21.bmp
# End Source File
# Begin Source File

SOURCE=.\media1.bmp
# End Source File
# Begin Source File

SOURCE=.\media2.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
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
# Begin Source File

SOURCE=..\..\CommonFiles\ReleaseDll\Win32Utils.lib
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="klop_9x" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=klop_9x - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klop_9x.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klop_9x.mak" CFG="klop_9x - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klop_9x - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "klop_9x - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Klop/win9x", PIZMCAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klop_9x - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../temp/release/klop_9x"
# PROP Intermediate_Dir "../../../../temp/release/klop_9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /O2 /I "$(VTOOLSD)\include" /I "..\..\KLSDK" /I "..\.." /D "ISWIN9X" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /D "WIN410_OR_LATER" /D NTVERSION=WDM /D i386=1 /D "NDIS40" /D "NDIS40_MINIPORT" /FAs /Zl /FD /Gs /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfms.lib /nologo /machine:IX86 /nodefaultlib /out:"Release\klop.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\Release\klop.vxd
TargetName=klop
SOURCE="$(InputPath)"
PreLink_Desc=PreBuild
PreLink_Cmds=PreBuild.cmd Release
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd Release $(TargetPath) $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "klop_9x - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../temp/debug/klop_9x"
# PROP Intermediate_Dir "../../../../temp/debug/klop_9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /W3 /Zi /Oi /Gy /I "$(VTOOLSD)\include" /I "..\..\KLSDK" /D "_DEBUG" /D "ISWIN9X" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /D "WIN410_OR_LATER" /D NTVERSION=WDM /D i386=1 /D "NDIS40" /D "NDIS40_MINIPORT" /FR /Zl /FD /Gs /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icrtms.obj daams.lib clms4.lib ndms95d.lib rtmsd.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfmsd.lib /nologo /incremental:no /debug /machine:IX86 /nodefaultlib /out:"Debug\klop.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\Debug\klop.vxd
TargetName=klop
SOURCE="$(InputPath)"
PreLink_Cmds=PreBuild.cmd Debug
PostBuild_Cmds=PostBuild.cmd Debug $(TargetPath) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "klop_9x - Win32 Release"
# Name "klop_9x - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\g_cp_proto.cpp
# End Source File
# Begin Source File

SOURCE=..\g_custom_packet.cpp
# End Source File
# Begin Source File

SOURCE=..\g_dispatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\g_key.cpp
# End Source File
# Begin Source File

SOURCE=..\g_klop_ndis.cpp
# End Source File
# Begin Source File

SOURCE=.\hook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_registry.cpp
# End Source File
# Begin Source File

SOURCE=..\klopevent.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\md5.cpp
# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\g_cp_proto.h
# End Source File
# Begin Source File

SOURCE=..\g_custom_packet.h
# End Source File
# Begin Source File

SOURCE=..\g_dispatcher.h
# End Source File
# Begin Source File

SOURCE=..\g_key.h
# End Source File
# Begin Source File

SOURCE=..\g_klop_ndis.h
# End Source File
# Begin Source File

SOURCE=.\hook.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_registry.h
# End Source File
# Begin Source File

SOURCE=..\..\HOOK\klop_api.h
# End Source File
# Begin Source File

SOURCE=..\klopevent.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# End Group
# Begin Group "KLSDK source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\KLSDK\g_thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_ioctl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_list.cpp
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_lock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_object.cpp
# End Source File
# End Group
# Begin Group "KLSDK include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\KLSDK\g_callback.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\g_plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\g_thread.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\Ids_common.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_DebugMask.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_ioctl.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_list.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_lock.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_mm.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_object.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kldef.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\klstatus.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\w9xdef.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\changes.txt
# End Source File
# Begin Source File

SOURCE=.\klop.DEF
# End Source File
# Begin Source File

SOURCE=.\KLOP.vrc
# End Source File
# Begin Source File

SOURCE=.\PostBuild.cmd
# End Source File
# Begin Source File

SOURCE=.\PreBuild.cmd
# End Source File
# End Target
# End Project

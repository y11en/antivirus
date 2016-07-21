# Microsoft Developer Studio Project File - Name="klpf9x" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=klpf9x - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klpf9x.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klpf9x.mak" CFG="klpf9x - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klpf9x - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "klpf9x - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Plugins/Klpf/WinNT", IQYHAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klpf9x - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /GX /O2 /I "$(VTOOLSD)\include" /I "..\..\..\KLSDK" /D "isWIN9x" /D "ISWIN9X" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /D "WIN410_OR_LATER" /D NTVERSION=WDM /D i386=1 /Zl /FD /Gs /c
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
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfms.lib /nologo /pdb:none /debug /debugtype:both /machine:IX86 /out:"Release\klpf.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
TargetPath=.\Release\klpf.vxd
TargetName=klpf
SOURCE="$(InputPath)"
PreLink_Desc=PreBuild
PreLink_Cmds=PreBuild.cmd Release
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd Release $(TargetPath) $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "klpf9x - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MT /W3 /Zi /Od /I "$(VTOOLSD)\include" /I "..\..\..\KLSDK" /D "_DEBUG" /D "ISWIN9X" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /D "WIN410_OR_LATER" /D NTVERSION=WDM /D i386=1 /FR /Zl /FD /Gs /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icrtms.obj daams.lib clms4.lib ndms95d.lib rtmsd.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfmsd.lib /nologo /incremental:no /debug /machine:IX86 /out:"Debug\klpf.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
TargetPath=.\Debug\klpf.vxd
TargetName=klpf
SOURCE="$(InputPath)"
PreLink_Cmds=PreBuild.cmd Debug
PostBuild_Cmds=PostBuild.cmd Debug $(TargetPath) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "klpf9x - Win32 Release"
# Name "klpf9x - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\handlers.c
# End Source File
# Begin Source File

SOURCE=.\Main.c
# End Source File
# Begin Source File

SOURCE=..\..\reg_plugin.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\g_precomp.h
# End Source File
# Begin Source File

SOURCE=..\handlers.h
# End Source File
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# Begin Source File

SOURCE=..\..\reg_plugin.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\klpf.DEF
# End Source File
# Begin Source File

SOURCE=.\klpf.VRC
# End Source File
# Begin Source File

SOURCE=.\PostBuild.cmd
# End Source File
# Begin Source File

SOURCE=.\PreBuild.cmd
# End Source File
# End Target
# End Project

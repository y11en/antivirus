# Microsoft Developer Studio Project File - Name="w9x" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=w9x - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "w9x.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "w9x.mak" CFG="w9x - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "w9x - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "w9x - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/Avp_IO", ZHDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "w9x - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir "..\..\..\temp\Release9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G3 /Zp1 /MT /W3 /O2 /Oy- /I "$(VTOOLSD)\include" /I "..\..\..\CommonFiles" /D "NDEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /D "_HOOK_VXD_" /Zl /FD /Gs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfms.lib /nologo /pdb:none /machine:IX86 /nodefaultlib /out:"..\Release9x/klif.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA
# Begin Special Build Tool
TargetName=klif
SOURCE="$(InputPath)"
PreLink_Cmds=PreBuild.cmd Release
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd Release ..\Release9x\klif.vxd $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "w9x - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "..\..\..\temp\Debug9x"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Zi /Od /Gf /I "$(VTOOLSD)\include" /I "..\..\..\CommonFiles" /D "DEBUG" /D "_DEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "VTOOLSD" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /D "_HOOK_VXD_" /FD /Gs /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib cfms.lib /nologo /incremental:no /pdb:"..\Debug9x/klif.pdb" /debug /machine:IX86 /nodefaultlib /out:"..\Debug9x\klif.vxd" /libpath:"$(VTOOLSD)\lib" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetName=klif
SOURCE="$(InputPath)"
PreLink_Desc=prelink...
PreLink_Cmds=PreBuild.cmd Debug
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd Debug ..\Debug9x\klif.vxd $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "w9x - Win32 Release"
# Name "w9x - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AVPG.DEF
# End Source File
# Begin Source File

SOURCE=.\cd9x.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\client.c
# End Source File
# Begin Source File

SOURCE=..\debug.c
# End Source File
# Begin Source File

SOURCE=..\evqueue.c
# End Source File
# Begin Source File

SOURCE=..\filter.c
# End Source File
# Begin Source File

SOURCE=.\firewall.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\i21.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ifs.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\InvThread.c
# End Source File
# Begin Source File

SOURCE=.\io.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ios.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LLDiskIO.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\NameCache.c
# End Source File
# Begin Source File

SOURCE=..\osspec.c
# End Source File
# Begin Source File

SOURCE=.\r3.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\registry.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\nt\runproc.c
# End Source File
# Begin Source File

SOURCE=.\specfpr.c
# End Source File
# Begin Source File

SOURCE=..\syscalls.c
# End Source File
# Begin Source File

SOURCE=..\tspv2.c
# End Source File
# Begin Source File

SOURCE=.\VXDHOOK.C
# End Source File
# Begin Source File

SOURCE=.\WINS.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cd9x.h
# End Source File
# Begin Source File

SOURCE=..\debug.h
# End Source File
# Begin Source File

SOURCE=..\filter.h
# End Source File
# Begin Source File

SOURCE=.\specfpr.h
# End Source File
# Begin Source File

SOURCE=.\vxdhook.h
# End Source File
# Begin Source File

SOURCE=.\wins.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\AVPG.VRC
# End Source File
# Begin Source File

SOURCE=.\PostBuild.cmd
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PreBuild.cmd
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\w9x.dep
# End Source File
# Begin Source File

SOURCE=.\w9x.mak
# End Source File
# End Target
# End Project

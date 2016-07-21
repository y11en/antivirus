# Microsoft Developer Studio Project File - Name="Avp_io VxD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Avp_io VxD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avp_io VxD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp_io VxD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Avp_io VxD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/Avp_IO", ZHDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avp_io VxD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release"
# PROP Intermediate_Dir "..\..\temp\Release\avpiovxd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /GB /Zp1 /MT /W3 /Zi /I "..\..\CommonFiles" /I "..\vtoolsd\include" /I "$(VTOOLSD)\include" /D "NDEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /Zl /FD /Gs /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 icrtms.obj clms4.lib daams.lib ndms95.lib rtms.lib ntms.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib /nologo /debug /machine:IX86 /nodefaultlib /def:".\Avp_io.def" /out:"../../out/release/AVP_IO.vxd" /libpath:"$(VTOOLSD)\lib" /libpath:"..\..\commonfiles\release" /IGNORE:4078,4039,4070 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\release\AVP_IO.vxd
TargetName=AVP_IO
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd RELEASE $(TargetPath) $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Avp_io VxD - Win32 Debug"

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
# ADD CPP /nologo /G3 /Zp1 /MTd /W3 /Gm /Zi /Od /I "..\..\CommonFiles" /I "..\vtoolsd\include" /I "$(VTOOLSD)\include" /D "DEBUG" /D "_DEBUG" /D "_X86_" /D "IS_32" /D "WIN40" /D "WANTVXDWRAPS" /D "ALT_INIT_SEG" /D "WIN40_OR_LATER" /D "WFW311_OR_LATER" /FD /Gs /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 icrtms.obj clms4d.lib daamsd.lib ndms95d.lib rtmsd.lib ntmsd.lib wr0ms.lib wr1ms.lib wr2ms.lib wr3ms.lib chkesp.obj /nologo /debug /machine:IX86 /nodefaultlib /def:".\Avp_io.def" /out:"../../out/debug/AVP_IO.vxd" /libpath:"$(VTOOLSD)\lib" /libpath:"..\..\commonfiles\release" /IGNORE:4078,4039,4070,4075 /VXD /MERGE:.data=_LDATA /MERGE:ICRTTEXT=_IDATA /MERGE:ICRTXXXX=_IDATA /MERGE:.rdata=_LDATA /MERGE:.bss=_LDATA /MERGE:_PDATA=_PTEXT /MERGE:.CRT=_LTEXT /MERGE:.rsrc=_IDATA
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\debug\AVP_IO.vxd
TargetName=AVP_IO
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=PostBuild.cmd DEBUG $(TargetPath) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Avp_io VxD - Win32 Release"
# Name "Avp_io VxD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Avp_io.cpp
# End Source File
# Begin Source File

SOURCE=.\Avp_io.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\C_vxdio.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=.\Avp_io.h
# End Source File
# Begin Source File

SOURCE=..\Avp_iocl.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Avp_io.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Avp_io.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=".\Avp_io VxD.dep"
# End Source File
# Begin Source File

SOURCE=".\Avp_io VxD.mak"
# End Source File
# Begin Source File

SOURCE=.\PostBuild.cmd
# End Source File
# End Target
# End Project

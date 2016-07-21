# Microsoft Developer Studio Project File - Name="Avp_ioNT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Avp_ioNT - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Avp_iont.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp_iont.mak" CFG="Avp_ioNT - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp_ioNT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Avp_ioNT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/Avp_IO", ZHDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Avp_ioNT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Avp_ioNT"
# PROP BASE Intermediate_Dir ".\Avp_ioNT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\Release"
# PROP Intermediate_Dir "..\..\temp\Release/avpiont"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /Zi /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib msvcrt.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /def:".\Avp_ioNT.def" /ALIGN:4096 /IGNORE:4108
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\out\Release\Avp_iont.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner  $(TargetPath)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Avp_ioNT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Avp_ioN0"
# PROP BASE Intermediate_Dir ".\Avp_ioN0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\Avp_ioNT.def" /out:"../../out/debug/Avp_iont.dll"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Special Build Tool
TargetPath=\out\debug\Avp_iont.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
PostBuild_Cmds=tsigner  $(TargetPath)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Avp_ioNT - Win32 Release"
# Name "Avp_ioNT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Avp_ioNT.c
# End Source File
# Begin Source File

SOURCE=.\Avp_ioNT.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ntllio.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\avp_io32.h
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Version\ver_avp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Avp_ioNT.rc
# End Source File
# Begin Source File

SOURCE=.\Avp_ioNT.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\Avp_iont.dep
# End Source File
# Begin Source File

SOURCE=.\Avp_iont.mak
# End Source File
# End Target
# End Project

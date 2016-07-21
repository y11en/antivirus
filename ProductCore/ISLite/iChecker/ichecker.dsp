# Microsoft Developer Studio Project File - Name="ichecker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ichecker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ichecker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ichecker.mak" CFG="ichecker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ichecker - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ichecker - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/ISLite/ichecker", KFLCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ichecker - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/ISLite/iChecker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/ISLite/iChecker"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICHECKER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICHECKER_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0xAD200000" /dll /pdb:"../../../out/Release/ichecker.pdb" /map:"../../../out/Release/ichecker.map" /debug /machine:I386 /out:"../../../out/Release/ichecker.ppl" /pdbtype:sept /ALIGN:4096 /IGNORE:4108
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
TargetDir=\Prague\Release
TargetPath=\Prague\Release\ichecker.ppl
TargetName=ichecker
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"	addsym $(TargetDir)\$(TargetName).pdb $(TargetName)
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ichecker - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/ISLite/iChecker"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/ISLite/iChecker"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICHECKER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICHECKER_EXPORTS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0xAD200000" /dll /pdb:"../../../out/Debug/ichecker.pdb" /debug /machine:I386 /out:"../../../out/Debug/ichecker.ppl" /pdbtype:sept /IGNORE:6004
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetDir=\Prague\Debug
TargetName=ichecker
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=addsym $(TargetDir)\$(TargetName).pdb $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "ichecker - Win32 Release"
# Name "ichecker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\HASH_API\MD5\hash_md5.c
# End Source File
# Begin Source File

SOURCE=.\ifilesec.c
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_ifilesec.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\HASH_API\CRAPI\crapi.h
# End Source File
# Begin Source File

SOURCE=.\ELF.H
# End Source File
# Begin Source File

SOURCE=.\ENTR_ELF.H
# End Source File
# Begin Source File

SOURCE=.\ifilesec.h
# End Source File
# Begin Source File

SOURCE=.\itss.h
# End Source File
# Begin Source File

SOURCE=.\NEWEXE.H
# End Source File
# Begin Source File

SOURCE=.\PEHDR.H
# End Source File
# Begin Source File

SOURCE=.\plugin_ifilesec.h
# End Source File
# Begin Source File

SOURCE=.\rar.h
# End Source File
# Begin Source File

SOURCE=.\zip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ifilesec.meta
# End Source File
# Begin Source File

SOURCE=.\ifilesec.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\ichecker.dep
# End Source File
# Begin Source File

SOURCE=.\ichecker.mak
# End Source File
# Begin Source File

SOURCE=.\ifilesec.imp
# End Source File
# Begin Source File

SOURCE=..\..\Include\Prt\iFileSec.prt
# End Source File
# End Target
# End Project

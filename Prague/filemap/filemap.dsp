# Microsoft Developer Studio Project File - Name="filemap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=filemap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "filemap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "filemap.mak" CFG="filemap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "filemap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "filemap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/KLAVA/klava/kernel/filemap", FWLIAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "filemap - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Out\Release"
# PROP Intermediate_Dir "../../temp/Release/prague/filemap"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "klavlib_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O2 /I "..\..\..\Prague/include" /I "..\include" /I "..\..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "klavlib_EXPORTS" /D "C_STYLE_PROP" /D "_PRAGUE_TRACE_FRAME_" /FR /FD /c
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
# ADD LINK32 kernel32.lib klsys.lib /nologo /entry:"DllMain" /dll /debug /machine:I386 /nodefaultlib:"kernel32.lib user32.lib advapi32.lib" /out:"..\..\Out\Release/filemap.ppl" /libpath:"..\..\Out\Release" /ALIGN:4096 /IGNORE:4108 /opt:ref /opt:icf
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\Out\Release\filemap.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Converting and signing component...
PostBuild_Cmds=call tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "filemap - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Out\Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/filemap"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "klavlib_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Zi /Od /I "..\include" /I "..\..\CommonFiles" /D "_MBCS" /D "_USRDLL" /D "klavlib_EXPORTS" /D "C_STYLE_PROP" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\commonfiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib advapi32.lib kernel32.lib klsys.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"..\..\Out\Debug/filemap.ppl" /libpath:"..\..\Out\Debug"
# SUBTRACT LINK32 /pdb:none /map /nodefaultlib /force

!ENDIF 

# Begin Target

# Name "filemap - Win32 Release"
# Name "filemap - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\filemap.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_filemapping.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\filemap.meta
# End Source File
# Begin Source File

SOURCE=.\filemap.rc

!IF  "$(CFG)" == "filemap - Win32 Release"

# ADD BASE RSC /l 0x419
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles"

!ELSEIF  "$(CFG)" == "filemap - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project

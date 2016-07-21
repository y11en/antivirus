# Microsoft Developer Studio Project File - Name="KLCKAH" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=KLCKAH - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KLCKAH.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KLCKAH.mak" CFG="KLCKAH - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KLCKAH - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "KLCKAH - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/Kaspersky Anti-Virus Personal 5.0 MP1/Components/Windows/KAH/KLCKAH", DQHLAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KLCKAH - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\temp\Release\KLCKAH"
# PROP Intermediate_Dir "..\..\..\..\temp\Release\KLCKAH"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCKAH_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\..\..\CommonFiles" /I "..\..\..\..\CS AdminKit\development" /I "..\..\..\..\CS AdminKit\development\include" /I "..\..\..\..\Prague\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCKAH_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ckahum.lib ws2_32.lib pr_rmt.lib fsdrvlib.lib ckahrule.lib /nologo /dll /pdb:"..\..\..\..\out\Release\KLCKAH.pdb" /debug /machine:I386 /out:"..\..\..\..\out\Release\KLCKAH.dll" /implib:"..\..\..\..\out\Release\KLCKAH.lib" /pdbtype:sept /libpath:"../../../../out/release"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\Release\Kaspersky Anti-Virus Personal 5.0\out\Release\KLCKAH.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "KLCKAH - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\temp\Debug\KLCKAH"
# PROP Intermediate_Dir "..\..\..\..\temp\Debug\KLCKAH"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCKAH_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\CommonFiles" /I "..\..\..\..\CS AdminKit\development" /I "..\..\..\..\CS AdminKit\development\include" /I "..\..\..\..\Prague\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KLCKAH_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ckahum.lib ws2_32.lib pr_rmt.lib fsdrvlib.lib ckahrule.lib /nologo /dll /pdb:"..\..\..\..\out\Debug\KLCKAH.pdb" /debug /machine:I386 /out:"..\..\..\..\out\Debug\KLCKAH.dll" /implib:"..\..\..\..\out\Debug\KLCKAH.lib" /pdbtype:sept /libpath:"../../../../out/debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "KLCKAH - Win32 Release"
# Name "KLCKAH - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CKAHSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHStatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\CKAHTask.cpp
# End Source File
# Begin Source File

SOURCE=.\KLCKAH.cpp
# End Source File
# Begin Source File

SOURCE=.\klckah.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonFiles\CCClient\Policy.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonFiles\Stuff\RegStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ckah_rptdefs.h
# End Source File
# Begin Source File

SOURCE=.\CKAHNotify.h
# End Source File
# Begin Source File

SOURCE=.\CKAHSettings.h
# End Source File
# Begin Source File

SOURCE=.\CKAHStatistics.h
# End Source File
# Begin Source File

SOURCE=.\CKAHTask.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\CommonFiles\Stuff\RegStorage.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TraceLevels.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# End Target
# End Project

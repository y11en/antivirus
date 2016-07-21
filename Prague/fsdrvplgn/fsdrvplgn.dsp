# Microsoft Developer Studio Project File - Name="FSDrvPlgn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FSDrvPlgn - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fsdrvplgn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fsdrvplgn.mak" CFG="FSDrvPlgn - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSDrvPlgn - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSDrvPlgn - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSDrvPlgn - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/fsdrvplgn", GSMXAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FSDrvPlgn - Win32 ReleaseWithTrace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/ReleaseWithTrace"
# PROP BASE Intermediate_Dir "../../temp/ReleaseWithTrace/prague/fsdrvplgn"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/ReleaseWithTrace"
# PROP Intermediate_Dir "../../temp/ReleaseWithTrace/prague/fsdrvplgn"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:"../../out/ReleaseWithTrace/fsdrvplgn.ppl" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\ReleaseWithTrace\fsdrvplgn.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSDrvPlgn - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/fsdrvplgn"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/fsdrvplgn"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\Include" /I "../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib kernel32.lib advapi32.lib  /nologo /base:"0x63a00000" /subsystem:windows /dll /debug /machine:I386 /out:"../../out/Release/fsdrvplgn.ppl" /libpath:"..\..\out\release" /libpath:"..\..\CommonFiles\ReleaseDLL" /opt:ref
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\Release\fsdrvplgn.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSDrvPlgn - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/fsdrvplgn"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/fsdrvplgn"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\Include" /I "../../CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib kernel32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../out/Debug/fsdrvplgn.ppl" /implib:"" /pdbtype:sept /libpath:"..\..\out\debug"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
TargetPath=\out\Debug\fsdrvplgn.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FSDrvPlgn - Win32 ReleaseWithTrace"
# Name "FSDrvPlgn - Win32 Release"
# Name "FSDrvPlgn - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\plugin_fsdrvplgn.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Include\plugin\p_fsdrvplgn.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fsdrvplgn.dsc
# End Source File
# Begin Source File

SOURCE=.\fsdrvplgn.imp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\fsdrvplgn.meta
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\fsdrvplgn.rc
# End Source File
# End Group
# End Target
# End Project

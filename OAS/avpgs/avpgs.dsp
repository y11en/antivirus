# Microsoft Developer Studio Project File - Name="avpg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=avpg - Win32 Debug WithoutStopping
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "avpgs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avpgs.mak" CFG="avpg - Win32 Debug WithoutStopping"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avpg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avpg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avpg - Win32 Debug WithoutStopping" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/avpgs", EVBBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "avpg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../out/Release"
# PROP BASE Intermediate_Dir "../../temp/Release/prague/avpgs"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/prague/avpgs"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib sign.lib /nologo /base:"0x61f00000" /dll /debug /machine:I386 /def:".\avpgs.def" /out:"../../out/Release/avpgs.ppl" /libpath:"..\..\out\Release" /libpath:"..\..\CommonFiles\ReleaseDll\\" /opt:ref
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
TargetPath=\out\Release\avpgs.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug"
# PROP BASE Intermediate_Dir "../../temp/Debug/prague/avpgs"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/prague/avpgs"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /Zi /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib sign.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug/avpgs.ppl" /pdbtype:sept /libpath:"..\..\out\Debug" /libpath:"..\..\CommonFiles\DebugDll\\" /IGNORE:6004
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug WithoutStopping"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../out/Debug WithoutStopping"
# PROP BASE Intermediate_Dir "../../temp/Debug WithoutStopping/prague/avpgs"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug WithoutStopping"
# PROP Intermediate_Dir "../../temp/Debug WithoutStopping/prague/avpgs"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /Zi /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPG_EXPORTS" /D "_PRAGUE_TRACE_" /D "_WITHOUTSTOPPING_" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug WithoutStopping/avpgs.ppl" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll\\"
# SUBTRACT BASE LINK32 /pdb:none /incremental:no
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib FSDrvLib.lib sign.lib /nologo /base:"0xAD100000" /dll /debug /machine:I386 /out:"../../out/Debug/avpgs.ppl" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll\\"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Special Build Tool
TargetPath=\out\Debug\avpgs.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Sign "avpg" plugin
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "avpg - Win32 Release"
# Name "avpg - Win32 Debug"
# Name "avpg - Win32 Debug WithoutStopping"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\avpg.cpp
# End Source File
# Begin Source File

SOURCE=.\avpgimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\avpgs.def

!IF  "$(CFG)" == "avpg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug"

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug WithoutStopping"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\DrvEventList.cpp
# End Source File
# Begin Source File

SOURCE=.\EvCache.cpp
# End Source File
# Begin Source File

SOURCE=.\exphooks.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Hook\R3Hook\hookbase.c
# End Source File
# Begin Source File

SOURCE=.\job_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\OwnSync.cpp
# End Source File
# Begin Source File

SOURCE=..\..\windows\Hook\R3Hook\ParseExports.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_avpg.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\avpgimpl.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\DefList.h
# End Source File
# Begin Source File

SOURCE=.\DrvEventList.h
# End Source File
# Begin Source File

SOURCE=.\EvCache.h
# End Source File
# Begin Source File

SOURCE=.\exphooks.h
# End Source File
# Begin Source File

SOURCE=.\exphooks_list.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Hook\R3Hook\hookbase.h
# End Source File
# Begin Source File

SOURCE=.\job_thread.h
# End Source File
# Begin Source File

SOURCE=.\OwnSync.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\TaskThread.h
# End Source File
# Begin Source File

SOURCE=..\Include\common\UserBan.h
# End Source File
# Begin Source File

SOURCE=.\VER_MOD.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Avpg.imp

!IF  "$(CFG)" == "avpg - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug WithoutStopping"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Avpg.meta

!IF  "$(CFG)" == "avpg - Win32 Release"

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "avpg - Win32 Debug WithoutStopping"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\avpg.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\avpgs.dep
# End Source File
# Begin Source File

SOURCE=.\avpgs.mak
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="FSSync" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FSSync - Win32 DebugS AutoThreadReg
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FSSync.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FSSync.mak" CFG="FSSync - Win32 DebugS AutoThreadReg"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSSync - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 DebugS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 ReleaseS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSSync - Win32 DebugS AutoThreadReg" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/FSSync", RRSDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FSSync - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "..\..\..\temp\Release\fsdrv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\Hook" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /base:"0x63b00000" /dll /pdb:"..\..\..\out\Release/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\Release/FSSync.dll" /implib:"..\..\..\out\Release/FSSync.lib" /libpath:"..\..\..\out\Release" /libpath:"..\..\..\CommonFiles\ReleaseDll" /opt:ref
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "..\..\..\temp\Debug\fsdrv"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\Hook" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 FSDrvLib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:"..\..\..\out\Debug/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\Debug/FSSync.dll" /implib:"..\..\..\out\Debug/FSSync.lib" /pdbtype:sept /libpath:"..\..\..\out\Debug" /libpath:"..\..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDll"
# PROP BASE Intermediate_Dir "DebugDll"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugS"
# PROP Intermediate_Dir "..\..\..\temp\DebugS\fsdrv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDll"
# ADD LINK32 sign.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /pdb:"..\..\..\out\DebugS/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\DebugS/FSSync.dll" /implib:"..\..\..\out\DebugS\FSSync.lib" /pdbtype:sept /libpath:"..\..\..\out\DebugS" /libpath:"..\..\..\CommonFiles\Debug"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\DebugS\FSSync.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=call tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDll"
# PROP BASE Intermediate_Dir "ReleaseDll"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseS"
# PROP Intermediate_Dir "..\..\..\temp\ReleaseS\fsdrv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zd /O2 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /machine:I386 /libpath:"..\..\..\..\CommonFiles\ReleaseDll"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib sign.lib /nologo /dll /pdb:"..\..\..\out\ReleaseS/FSSync.pdb" /debug /machine:I386 /def:".\FSSync.def" /out:"..\..\..\out\ReleaseS\FSSync.dll" /implib:"..\..\..\out\ReleaseS/FSSync.lib" /libpath:"..\..\..\out\ReleaseS" /libpath:"..\..\..\CommonFiles\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\ReleaseS\FSSync.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=call tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FSSync___Win32_DebugDll_AutoThreadReg"
# PROP BASE Intermediate_Dir "FSSync___Win32_DebugDll_AutoThreadReg"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "FSSync___Win32_DebugDll_AutoThreadReg"
# PROP Intermediate_Dir "FSSync___Win32_DebugDll_AutoThreadReg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\CommonFiles" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FSSYNC_EXPORTS" /D "FSDRV_AUTOTHREADREGISTER" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 sign.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /pdb:"..\..\..\CommonFiles\DebugDll/FSSync.pdb" /debug /machine:I386 /out:"..\..\..\CommonFiles\DebugDll/FSSync.dll" /implib:"..\..\..\CommonFiles\DebugDll\FSSync.lib" /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDll"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 sign.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /dll /pdb:"..\..\..\CommonFiles\DebugDll/FSSync.pdb" /debug /machine:I386 /out:"..\..\..\CommonFiles\DebugDll/FSSync.dll" /implib:"..\..\..\CommonFiles\DebugDll\FSSync.lib" /pdbtype:sept /libpath:"..\..\..\CommonFiles\DebugDll"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\CommonFiles\DebugDll\FSSync.dll
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"	PostBuild DebugDll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FSSync - Win32 Release"
# Name "FSSync - Win32 Debug"
# Name "FSSync - Win32 DebugS"
# Name "FSSync - Win32 ReleaseS"
# Name "FSSync - Win32 DebugS AutoThreadReg"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\proactive\DrvEventList.cpp
# End Source File
# Begin Source File

SOURCE=.\FSSync.cpp
# End Source File
# Begin Source File

SOURCE=.\FSSync.def

!IF  "$(CFG)" == "FSSync - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\proactive\job_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\proactive\OwnSync.cpp
# End Source File
# Begin Source File

SOURCE=.\proactive\proactive.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\proactive\TaskThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\proactive\DefList.h
# End Source File
# Begin Source File

SOURCE=.\proactive\DrvEventList.h
# End Source File
# Begin Source File

SOURCE=.\proactive\job_thread.h
# End Source File
# Begin Source File

SOURCE=.\proactive\OwnSync.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\proactive\TaskThread.h
# End Source File
# Begin Source File

SOURCE=.\VER_MOD.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\FSSync.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\FSSync.dep

!IF  "$(CFG)" == "FSSync - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FSSync.mak

!IF  "$(CFG)" == "FSSync - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 ReleaseS"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "FSSync - Win32 DebugS AutoThreadReg"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

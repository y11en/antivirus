# Microsoft Developer Studio Project File - Name="det_act" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=det_act - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "det_act.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "det_act.mak" CFG="det_act - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "det_act - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "det_act - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "det_act - Win32 Release wtih dbgout" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/TestProjects/det_act", SPSIAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "det_act - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "..\..\..\..\..\prague\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib fsdrvlib.lib Advapi32.lib shell32.lib TreeSearch.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"..\..\..\..\CommonFiles\ReleaseDll" /libpath:"..\..\..\..\Release\KLAVA\Out\Release" /opt:ref
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "det_act - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\prague\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_NO_BLOCK" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib fsdrvlib.lib Advapi32.lib shell32.lib TreeSearch.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll" /libpath:"..\..\..\..\Release\KLAVA\Out\Debug"

!ELSEIF  "$(CFG)" == "det_act - Win32 Release wtih dbgout"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "det_act___Win32_Release_wtih_dbgout"
# PROP BASE Intermediate_Dir "det_act___Win32_Release_wtih_dbgout"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "det_act___Win32_Release_wtih_dbgout"
# PROP Intermediate_Dir "det_act___Win32_Release_wtih_dbgout"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "..\..\..\..\..\prague\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "__DBGOUT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib fsdrvlib.lib Advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"..\..\..\..\out\Release" /opt:ref
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "det_act - Win32 Release"
# Name "det_act - Win32 Debug"
# Name "det_act - Win32 Release wtih dbgout"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ActivityCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\cmpimages.cpp
# End Source File
# Begin Source File

SOURCE=.\stuff\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\det_act.cpp
# End Source File
# Begin Source File

SOURCE=.\stuff\DrvEventList.cpp
# End Source File
# Begin Source File

SOURCE=.\job_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\stuff\mm.cpp
# End Source File
# Begin Source File

SOURCE=.\stuff\OwnSync.cpp
# End Source File
# Begin Source File

SOURCE=.\PathList.cpp
# End Source File
# Begin Source File

SOURCE=.\proclist.cpp
# End Source File
# Begin Source File

SOURCE=.\RegAutoRuns.cpp
# End Source File
# Begin Source File

SOURCE=.\RegHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\SelfCopyHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\stuff\servfuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\StrList.cpp
# End Source File
# Begin Source File

SOURCE=.\stuff\TaskThread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ActivityCheck.h
# End Source File
# Begin Source File

SOURCE=.\cmpimages.h
# End Source File
# Begin Source File

SOURCE=.\stuff\debug.h
# End Source File
# Begin Source File

SOURCE=.\stuff\DefList.h
# End Source File
# Begin Source File

SOURCE=.\stuff\DrvEventList.h
# End Source File
# Begin Source File

SOURCE=.\job_thread.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\stuff\mm.h
# End Source File
# Begin Source File

SOURCE=.\stuff\OwnSync.h
# End Source File
# Begin Source File

SOURCE=.\PathList.h
# End Source File
# Begin Source File

SOURCE=.\proclist.h
# End Source File
# Begin Source File

SOURCE=.\RegAutoRuns.h
# End Source File
# Begin Source File

SOURCE=.\RegHistory.h
# End Source File
# Begin Source File

SOURCE=.\SelfCopyHistory.h
# End Source File
# Begin Source File

SOURCE=.\stuff\servfuncs.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StrList.h
# End Source File
# Begin Source File

SOURCE=.\stuff\TaskThread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

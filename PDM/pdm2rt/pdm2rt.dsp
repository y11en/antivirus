# Microsoft Developer Studio Project File - Name="pdm2rt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pdm2rt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pdm2rt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pdm2rt.mak" CFG="pdm2rt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pdm2rt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pdm2rt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "pdm2rt"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pdm2rt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../out/Release"
# PROP Intermediate_Dir "../../temp/Release/ppp/pdm2rt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PDM2RT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "..\include" /I "..\..\prague\include" /I "..\..\CommonFiles" /I "$(WDK)\inc\ddk" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "PDM2RT_EXPORTS" /D "_PRAGUE_TRACE_" /D "_NOWDK_" /D TR_MODULE=pdm2rt /FD /c
# SUBTRACT CPP /FA<none>
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mklapi.lib fltLib.lib /nologo /dll /machine:I386 /out:"../../out/Release/pdm2rt.ppl" /pdbtype:sept /libpath:"..\..\out\mklif\fre\i386" /libpath:"$(WDK)\lib\w2k\i386" /OPT:REF
# SUBTRACT LINK32 /pdb:none /map /debug
# Begin Special Build Tool
TargetPath=\out\Release\pdm2rt.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "pdm2rt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../out/Debug"
# PROP Intermediate_Dir "../../temp/Debug/ppp/pdm2rt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PDM2RT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\prague\include" /I "..\..\CommonFiles" /I "$(WDK)\inc\ddk" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "PDM2RT_EXPORTS" /D "_PRAGUE_TRACE_" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../out/Debug/pdm2rt.ppl" /pdbtype:sept
# Begin Special Build Tool
TargetPath=\out\Debug\pdm2rt.ppl
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "pdm2rt - Win32 Release"
# Name "pdm2rt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\cpath.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\cregkey.cpp
# End Source File
# Begin Source File

SOURCE=.\env\envhelper_win32w.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\eventmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\heur_base.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\heur_invader.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\heur_killav.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\lock2l.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\mwlist.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\opcache.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM\OwnSync.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\p2p.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\pathlist.cpp
# End Source File
# Begin Source File

SOURCE=.\pdm2imp.cpp
# End Source File
# Begin Source File

SOURCE=.\pdm2rt_res.rc
# End Source File
# Begin Source File

SOURCE=.\plugin_pdm2rt.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\proclist.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\regar.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\sclist.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\srv.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\strlist.cpp
# End Source File
# Begin Source File

SOURCE=.\task.cpp
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\src\tstring.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\cpath.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\cregkey.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\envhelper.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\envhelper_pure.h
# End Source File
# Begin Source File

SOURCE=.\env\envhelper_win32w.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\eventhandler.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\eventmgr.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\heur_base.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\heur_invader.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\heur_killav.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\lock.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\mwlist.h
# End Source File
# Begin Source File

SOURCE=..\PDM\OwnSync.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\p2p.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\pathlist.h
# End Source File
# Begin Source File

SOURCE=.\pdm2imp.h
# End Source File
# Begin Source File

SOURCE=.\pdm2rt_res.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\pe.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\proclist.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\regar.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\sclist.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\srv.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\strlist.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\heuristic\trace.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\tstring.h
# End Source File
# Begin Source File

SOURCE=..\PDM2\eventmgr\include\types.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\pdm2rt.dsc
# End Source File
# Begin Source File

SOURCE=.\pdm2rt.imp
# End Source File
# Begin Source File

SOURCE=.\pdm2rt.meta
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="pdm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=pdm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pdm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pdm.mak" CFG="pdm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pdm - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "pdm - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "pdm"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pdm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../eventmgr/include" /I "../heuristic" /I "../../../commonfiles" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "pdm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../commonfiles" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "pdm - Win32 Release"
# Name "pdm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\eventmgr\src\cpath.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\cregkey.cpp
# End Source File
# Begin Source File

SOURCE=.\envhelper_win32w.cpp
# End Source File
# Begin Source File

SOURCE=.\eventhandler_pdm.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\eventmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_base.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic2\heur_dumb.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_hosts.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_install.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_invader.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_killav.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_killfiles.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_virus.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\lock2l.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\mwlist.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\opcache.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\p2p.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\pathlist.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\proclist.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\regar.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\sclist.cpp
# End Source File
# Begin Source File

SOURCE=..\heuristic\srv.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\strlist.cpp
# End Source File
# Begin Source File

SOURCE=..\eventmgr\src\tstring.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\eventmgr\include\_winnt.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\cpath.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\cregkey.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\envhelper.h
# End Source File
# Begin Source File

SOURCE=.\envhelper_win32w.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\eventhandler.h
# End Source File
# Begin Source File

SOURCE=.\eventhandler_pdm.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\eventmgr.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_all.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_av.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_base.h
# End Source File
# Begin Source File

SOURCE=..\heuristic2\heur_dumb.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_hosts.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_install.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_invader.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_killav.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_killfiles.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\heur_virus.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\lock.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\mwlist.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\opcache.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\opcachef.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\p2p.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\pathlist.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\pe.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\proclist.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\regar.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\sclist.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\srv.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\strlist.h
# End Source File
# Begin Source File

SOURCE=..\heuristic\trace.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\tstring.h
# End Source File
# Begin Source File

SOURCE=..\eventmgr\include\types.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\trace.h
# End Source File
# End Target
# End Project

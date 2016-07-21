# Microsoft Developer Studio Project File - Name="PlugTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PlugTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PlugTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PlugTest.mak" CFG="PlugTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PlugTest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PlugTest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PlugTest - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\KLSDK" /D "NDEBUG" /D "USER_MODE" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "PlugTest - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\KLSDK" /D "_DEBUG" /D "USER_MODE" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0400 /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
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

# Name "PlugTest - Win32 Release"
# Name "PlugTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\PlugTest.cpp
# End Source File
# Begin Source File

SOURCE=.\streamtest.cpp
# End Source File
# Begin Source File

SOURCE=.\streamtest.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CheckCKl_Buffer.h
# End Source File
# Begin Source File

SOURCE=.\CheckCKl_List.h
# End Source File
# Begin Source File

SOURCE=.\PlugTest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\ring3def.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "KLSDK Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\KLSDK\g_thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_alloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_list.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_lock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_map.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_mm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_object.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_sync.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_timer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_vector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\md5.cpp
# End Source File
# End Group
# Begin Group "KLSDK include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\KLSDK\g_thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_alloc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_DebugMask.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_filter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_list.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_list2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_lock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_map.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_mm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_string.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_sync.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kldef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\klstatus.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\md5.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

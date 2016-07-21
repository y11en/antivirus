# Microsoft Developer Studio Project File - Name="test_mime" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=test_mime - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_mime.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_mime.mak" CFG="test_mime - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_mime - Win32 Release_DMR" (based on "Win32 (x86) Console Application")
!MESSAGE "test_mime - Win32 Debug_DMD" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_mime - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "test_mime___Win32_Release_DMR"
# PROP BASE Intermediate_Dir "test_mime___Win32_Release_DMR"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../bin"
# PROP Intermediate_Dir "../tmp/MIMEParser/Release_DMR"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib shell32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib shell32.lib advapi32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "test_mime - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "test_mime___Win32_Debug_DMD"
# PROP BASE Intermediate_Dir "test_mime___Win32_Debug_DMD"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../bin"
# PROP Intermediate_Dir "../tmp/MIMEParser/Debug_DMD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib shell32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/test_mime_d.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib shell32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/test_mime_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "test_mime - Win32 Release_DMR"
# Name "test_mime - Win32 Debug_DMD"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\test_parser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MIMEParser.hpp
# End Source File
# End Group
# Begin Group "STL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\STL\stlport\stl\_config.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\stl\_config_compat.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\stl\_config_compat_post.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\_epilog.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\stl\_epilog.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\_msvc_warnings_off.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\stl\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\stl\_site_config.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\stl_confix.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\stl_msvc.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\stl_select_lib.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\stl_user_config.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\stlcomp.h
# End Source File
# Begin Source File

SOURCE=..\STL\stlport\config\vc_select_lib.h
# End Source File
# End Group
# End Target
# End Project

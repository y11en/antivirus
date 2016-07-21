# Microsoft Developer Studio Project File - Name="libmimeparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmimeparser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmimeparser_kis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmimeparser_kis.mak" CFG="libmimeparser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmimeparser - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libmimeparser - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmimeparser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libmimeparser___Win32_Debug"
# PROP BASE Intermediate_Dir "libmimeparser___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../kis/libd"
# PROP Intermediate_Dir "../tmp/kis/MIMEParser/Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"../bin/libmimeparser.bsc"
# ADD BSC32 /nologo /o"../bin/libmimeparser.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libmimeparser_dmd.lib"
# ADD LIB32 /nologo /out:"../kis/libd/libmimeparser.lib"

!ELSEIF  "$(CFG)" == "libmimeparser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libmimeparser___Win32_Release"
# PROP BASE Intermediate_Dir "libmimeparser___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../kis/lib"
# PROP Intermediate_Dir "../tmp/kis/MIMEParser/Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"../bin/libmimeparser.bsc"
# ADD BSC32 /nologo /o"../bin/libmimeparser.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libmimeparser_dmr.lib"
# ADD LIB32 /nologo /out:"../kis/lib/libmimeparser.lib"

!ENDIF 

# Begin Target

# Name "libmimeparser - Win32 Debug"
# Name "libmimeparser - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MIMEParser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MIMEParser.hpp
# End Source File
# Begin Source File

SOURCE=.\MIMEParser.link.h
# End Source File
# End Group
# Begin Group "STL"

# PROP Default_Filter ""
# End Group
# End Target
# End Project

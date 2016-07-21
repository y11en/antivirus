# Microsoft Developer Studio Project File - Name="libmimeparser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmimeparser - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmimeparser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmimeparser.mak" CFG="libmimeparser - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmimeparser - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libmimeparser - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libmimeparser - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE "libmimeparser - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmimeparser - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libmimeparser___Win32_Debug_DMD"
# PROP BASE Intermediate_Dir "libmimeparser___Win32_Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../tmp/MIMEParser/Debug_DMD"
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
# ADD LIB32 /nologo /out:"libmimeparser_dmd.lib"

!ELSEIF  "$(CFG)" == "libmimeparser - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libmimeparser___Win32_Debug_SMD"
# PROP BASE Intermediate_Dir "libmimeparser___Win32_Debug_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../tmp/MIMEParser/Debug_SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"../bin/libmimeparser.bsc"
# ADD BSC32 /nologo /o"../bin/libmimeparser.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libmimeparser_dmd.lib"
# ADD LIB32 /nologo /out:"libmimeparser_smd.lib"

!ELSEIF  "$(CFG)" == "libmimeparser - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libmimeparser___Win32_Release_DMR"
# PROP BASE Intermediate_Dir "libmimeparser___Win32_Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../tmp/MIMEParser/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"../bin/libmimeparser.bsc"
# ADD BSC32 /nologo /o"../bin/libmimeparser.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libmimeparser_smr.lib"
# ADD LIB32 /nologo /out:"libmimeparser_dmr.lib"

!ELSEIF  "$(CFG)" == "libmimeparser - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libmimeparser___Win32_Release_SMR"
# PROP BASE Intermediate_Dir "libmimeparser___Win32_Release_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../tmp/MIMEParser/Release_SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"../bin/libmimeparser.bsc"
# ADD BSC32 /nologo /o"../bin/libmimeparser.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libmimeparser_smr.lib"
# ADD LIB32 /nologo /out:"libmimeparser_smr.lib"

!ENDIF 

# Begin Target

# Name "libmimeparser - Win32 Debug_DMD"
# Name "libmimeparser - Win32 Debug_SMD"
# Name "libmimeparser - Win32 Release_DMR"
# Name "libmimeparser - Win32 Release_SMR"
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

# Microsoft Developer Studio Project File - Name="kulib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=kulib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kulib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kulib.mak" CFG="kulib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kulib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "kulib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kulib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/kulib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "kulib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/kulib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "kulib - Win32 Release"
# Name "kulib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\ku_str.c
# End Source File
# Begin Source File

SOURCE=.\src\kualloc.c
# End Source File
# Begin Source File

SOURCE=.\src\kucase.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_alias.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_bld.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_err.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_mbcs.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_utf16.c
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_utf8.c
# End Source File
# Begin Source File

SOURCE=.\src\kudata.c
# End Source File
# Begin Source File

SOURCE=.\src\kudata_int.c
# End Source File
# Begin Source File

SOURCE=.\src\kudata_mapfile.c
# End Source File
# Begin Source File

SOURCE=.\src\kuhash.c
# End Source File
# Begin Source File

SOURCE=.\src\kuinit.c
# End Source File
# Begin Source File

SOURCE=.\src\kumutex.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ku_defs.h
# End Source File
# Begin Source File

SOURCE=.\ku_platform.h
# End Source File
# Begin Source File

SOURCE=.\ku_pwin32.h
# End Source File
# Begin Source File

SOURCE=.\src\ku_str.h
# End Source File
# Begin Source File

SOURCE=.\src\ku_utf.h
# End Source File
# Begin Source File

SOURCE=.\src\kualloc.h
# End Source File
# Begin Source File

SOURCE=.\kucase.h
# End Source File
# Begin Source File

SOURCE=.\kuconv.h
# End Source File
# Begin Source File

SOURCE=.\kuconv_err.h
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_int.h
# End Source File
# Begin Source File

SOURCE=.\src\kuconv_mbcs.h
# End Source File
# Begin Source File

SOURCE=.\src\kudata.h
# End Source File
# Begin Source File

SOURCE=.\src\kudata_int.h
# End Source File
# Begin Source File

SOURCE=.\src\kudata_mapfile.h
# End Source File
# Begin Source File

SOURCE=.\src\kuhash.h
# End Source File
# Begin Source File

SOURCE=.\kuinit.h
# End Source File
# Begin Source File

SOURCE=.\src\kuinit_int.h
# End Source File
# Begin Source File

SOURCE=.\src\kumutex.h
# End Source File
# End Group
# End Target
# End Project

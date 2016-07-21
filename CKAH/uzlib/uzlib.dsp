# Microsoft Developer Studio Project File - Name="uzlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=uzlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "uzlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "uzlib.mak" CFG="uzlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "uzlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "uzlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Components/Windows/KAH/uzlib", TGQQAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "uzlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\temp\Release\uzlib"
# PROP Intermediate_Dir "..\..\..\..\temp\Release\uzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O1 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "WINDLL" /D "DLL" /D "UNZIPLIB" /D "USE_EF_UT_TIME" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\out\Release\uzlib.lib"

!ELSEIF  "$(CFG)" == "uzlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\temp\Debug\uzlib"
# PROP Intermediate_Dir "..\..\..\..\temp\Debug\uzlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /YX /FD /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /Z7 /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "WINDLL" /D "DLL" /D "UNZIPLIB" /D "USE_EF_UT_TIME" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\out\Debug\uzlib.lib"

!ENDIF 

# Begin Target

# Name "uzlib - Win32 Release"
# Name "uzlib - Win32 Debug"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=api.c
# End Source File
# Begin Source File

SOURCE=crc32.c
# End Source File
# Begin Source File

SOURCE=crctab.c
# End Source File
# Begin Source File

SOURCE=crypt.c
# End Source File
# Begin Source File

SOURCE=explode.c
# End Source File
# Begin Source File

SOURCE=extract.c
# End Source File
# Begin Source File

SOURCE=fileio.c
# End Source File
# Begin Source File

SOURCE=globals.c
# End Source File
# Begin Source File

SOURCE=inflate.c
# End Source File
# Begin Source File

SOURCE=list.c
# End Source File
# Begin Source File

SOURCE=match.c
# End Source File
# Begin Source File

SOURCE=.\win32\nt.c
# End Source File
# Begin Source File

SOURCE=process.c
# End Source File
# Begin Source File

SOURCE=unreduce.c
# End Source File
# Begin Source File

SOURCE=unshrink.c
# End Source File
# Begin Source File

SOURCE=.\uz.cpp
# End Source File
# Begin Source File

SOURCE=.\win32\win32.c
# End Source File
# Begin Source File

SOURCE=.\windll\windll.c
# End Source File
# Begin Source File

SOURCE=zipinfo.c
# End Source File
# End Group
# Begin Group "Header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\consts.h
# End Source File
# Begin Source File

SOURCE=.\crypt.h
# End Source File
# Begin Source File

SOURCE=.\windll\decs.h
# End Source File
# Begin Source File

SOURCE=.\ebcdic.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\inflate.h
# End Source File
# Begin Source File

SOURCE=.\win32\nt.h
# End Source File
# Begin Source File

SOURCE=.\windll\structs.h
# End Source File
# Begin Source File

SOURCE=.\ttyio.h
# End Source File
# Begin Source File

SOURCE=.\unzip.h
# End Source File
# Begin Source File

SOURCE=.\unzpriv.h
# End Source File
# Begin Source File

SOURCE=.\unzvers.h
# End Source File
# Begin Source File

SOURCE=.\win32\w32cfg.h
# End Source File
# Begin Source File

SOURCE=.\windll\windll.h
# End Source File
# Begin Source File

SOURCE=.\zip.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\windll\unziplib.def
# End Source File
# End Target
# End Project

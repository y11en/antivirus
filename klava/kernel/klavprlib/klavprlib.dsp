# Microsoft Developer Studio Project File - Name="klavprlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=klavprlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klavprlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klavprlib.mak" CFG="klavprlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klavprlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "klavprlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "klavprlib"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klavprlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\Release"
# PROP Intermediate_Dir "..\..\..\Temp\Release\klava\kernel\klavprlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /Zi /O2 /I "..\include" /I "..\..\..\CommonFiles" /I "..\..\..\Prague\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "klavprlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\..\Temp\Debug\klava\kernel\klavprlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Zi /Od /I "..\include" /I "..\..\..\CommonFiles" /I "..\..\..\Prague\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /GZ /c
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

# Name "klavprlib - Win32 Release"
# Name "klavprlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\pr_alloc.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_comp.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_dbloader.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_io.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_io_lib.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_kfb_dbloader.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_proc_ldr.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_sync.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_tempfile.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_util.cpp
# End Source File
# Begin Source File

SOURCE=.\pr_vmem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\klav_prague.h
# End Source File
# End Group
# End Target
# End Project

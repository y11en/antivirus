# Microsoft Developer Studio Project File - Name="DBCleanUp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DBCleanUp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DBCleanUp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DBCleanUp.mak" CFG="DBCleanUp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DBCleanUp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DBCleanUp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/AVP32/DBCleanUp", WLEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DBCleanUp - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /Zi /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=.\Release\DBCleanUp.lib
SOURCE="$(InputPath)"

"." : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -fDBCleanUpWinNT 
	nmake -fDBCleanUpWin32 
	nmake -fDBCleanUpWin9x 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "DBCleanUp - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
InputPath=.\Debug\DBCleanUp.lib
SOURCE="$(InputPath)"

"." : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nmake -fDBCleanUpWin32 CFG="DEBUG" 
	nmake -fDBCleanUpWinNT CFG="DEBUG" 
	nmake -fDBCleanUpWin9x CFG="DEBUG" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "DBCleanUp - Win32 Release"
# Name "DBCleanUp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DirIterators9x.cpp
# PROP Ignore_Default_Tool 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DBCleanUp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DBCleanUp.dep
# End Source File
# Begin Source File

SOURCE=.\DBCleanUp.mak
# End Source File
# Begin Source File

SOURCE=.\DBCleanUpWin32
# End Source File
# Begin Source File

SOURCE=.\DBCleanUpWin9x
# End Source File
# Begin Source File

SOURCE=.\DBCleanUpWinNT
# End Source File
# End Target
# End Project

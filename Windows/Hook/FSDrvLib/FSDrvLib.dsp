# Microsoft Developer Studio Project File - Name="FSDrvLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FSDrvLib - Win32 DebugS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FSDrvLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FSDrvLib.mak" CFG="FSDrvLib - Win32 DebugS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSDrvLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FSDrvLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FSDrvLib - Win32 DebugS" (based on "Win32 (x86) Static Library")
!MESSAGE "FSDrvLib - Win32 ReleaseS" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/FSDrvLib", CLFBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "..\..\..\temp\Release\fsdrv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\out\Release\FSDrvLib.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=PreBuild Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "..\..\..\temp\Debug\fsdrv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\out\Debug\FSDrvLib.lib"

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDll"
# PROP BASE Intermediate_Dir "DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugS"
# PROP Intermediate_Dir "..\..\..\temp\DebugS\fsdrv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\CommonFiles\Debug\FSDrvLib.lib"
# ADD LIB32 /nologo /out:"..\..\..\out\DebugS\FSDrvLib.lib"

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDll"
# PROP BASE Intermediate_Dir "ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseS"
# PROP Intermediate_Dir "..\..\..\temp\ReleaseS\fsdrv"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\CommonFiles\Release\FSDrvLib.lib"
# ADD LIB32 /nologo /out:"..\..\..\out\ReleaseS\FSDrvLib.lib"

!ENDIF 

# Begin Target

# Name "FSDrvLib - Win32 Release"
# Name "FSDrvLib - Win32 Debug"
# Name "FSDrvLib - Win32 DebugS"
# Name "FSDrvLib - Win32 ReleaseS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FSDrv.cpp

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\IDriverEx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Service\sa.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\hook\avpgcom.h
# End Source File
# Begin Source File

SOURCE=..\Hook\FSDrvLib.h
# End Source File
# Begin Source File

SOURCE=..\hook\hookspec.h
# End Source File
# Begin Source File

SOURCE=..\Hook\IDriver.h
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonFiles\Service\SA.H
# End Source File
# End Group
# Begin Source File

SOURCE=.\FSDrvLib.dep
# End Source File
# Begin Source File

SOURCE=.\FSDrvLib.mak
# End Source File
# End Target
# End Project

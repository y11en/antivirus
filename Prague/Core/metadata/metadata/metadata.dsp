# Microsoft Developer Studio Project File - Name="metadata" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=metadata - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "metadata.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "metadata.mak" CFG="metadata - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "metadata - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "metadata - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/metadata/metadata", KRMXAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "metadata - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/release"
# PROP Intermediate_Dir "../../../temp/release/prague/metadata/metadata"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../include" /I "../../../commonfiles/" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetName=metadata
InputPath=\out\release\metadata.lib
SOURCE="$(InputPath)"

"..\..\..\out\ReleaseS\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo       copy       "$(InputPath)"       ..\..\..\out\ReleaseS\  
	copy       "$(InputPath)"       ..\..\..\out\ReleaseS\  
	
# End Custom Build

!ELSEIF  "$(CFG)" == "metadata - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/debug"
# PROP Intermediate_Dir "../../../temp/debug/prague/metadata/metadata"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../commonfiles/" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetName=metadata
InputPath=\out\debug\metadata.lib
SOURCE="$(InputPath)"

"..\..\..\out\DebugS\$(TargetName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo       copy       "$(InputPath)"       ..\..\..\out\DebugS\  
	copy       "$(InputPath)"       ..\..\..\out\DebugS\  
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "metadata - Win32 Release"
# Name "metadata - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\md5c.c
# End Source File
# Begin Source File

SOURCE=.\metadata.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=..\metadata.h
# End Source File
# End Group
# End Target
# End Project

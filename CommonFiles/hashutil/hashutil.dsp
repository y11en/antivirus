# Microsoft Developer Studio Project File - Name="hashutil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=hashutil - Win32 DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hashutil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hashutil.mak" CFG="hashutil - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hashutil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hashutil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "hashutil - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "hashutil - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/hashutil", GCOBAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "hashutil - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/hashutil"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\hashutil.lib
InputName=hashutil
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "hashutil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/hashutil"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\hashutil.lib
InputName=hashutil
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "hashutil - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/hashutil"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\hashutil.lib
InputName=hashutil
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "hashutil - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/hashutil"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\hashutil.lib
InputName=hashutil
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "hashutil - Win32 Release"
# Name "hashutil - Win32 Debug"
# Name "hashutil - Win32 DebugDll"
# Name "hashutil - Win32 ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\crc16.c
# End Source File
# Begin Source File

SOURCE=.\crc32.c
# End Source File
# Begin Source File

SOURCE=.\crc32str.c
# End Source File
# Begin Source File

SOURCE=.\src16str.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\hashutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\hashutil.dep
# End Source File
# Begin Source File

SOURCE=.\hashutil.mak
# End Source File
# End Target
# End Project

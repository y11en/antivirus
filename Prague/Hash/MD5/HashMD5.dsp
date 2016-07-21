# Microsoft Developer Studio Project File - Name="HashMD5" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=HashMD5 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HashMD5.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HashMD5.mak" CFG="HashMD5 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HashMD5 - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HashMD5 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HashMD5 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/Hash/MD5", AVVCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HashMD5 - Win32 ReleaseWithTrace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/ReleaseWithTrace"
# PROP BASE Intermediate_Dir "../../../temp/ReleaseWithTrace/prague/Hash/MD5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/ReleaseWithTrace"
# PROP Intermediate_Dir "../../../temp/ReleaseWithTrace/prague/Hash/MD5"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /entry:"DllMain" /subsystem:windows /dll /machine:I386 /nodefaultlib /out:"../../../out/ReleaseWithTrace/HASHMD5.PPL" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
IntDir=.\../../../temp/ReleaseWithTrace/prague/Hash/MD5
TargetPath=\out\ReleaseWithTrace\HASHMD5.PPL
InputPath=\out\ReleaseWithTrace\HASHMD5.PPL
SOURCE="$(InputPath)"

"$(IntDir)\custombuilddone.flg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist "$(TargetPath)" echo Converting "$(TargetPath)" 
	if exist "$(TargetPath)" call prconverter /c /b "$(TargetPath)" 
	if exist "$(TargetPath)" echo Signing "$(TargetPath)"... 
	if exist "$(TargetPath)" call tsigner "$(TargetPath)" 
	echo It is safe to delete this file.>"$(IntDir)\custombuilddone.flg" 
	
# End Custom Build
# Begin Special Build Tool
TargetPath=\out\ReleaseWithTrace\HASHMD5.PPL
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "HashMD5 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/Hash/MD5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/Hash/MD5"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HASHMD5_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /base:"0x63f00000" /entry:"DllMain" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:"../../../out/Release/HASHMD5.PPL" /opt:ref
# SUBTRACT LINK32 /pdb:none /map /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\HASHMD5.PPL
SOURCE="$(InputPath)"
PostBuild_Cmds=call prconvert "$(TargetPath)"	tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "HashMD5 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/Hash/MD5"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/Hash/MD5"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/HASHMD5.PPL" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /verbose /pdb:none /nodefaultlib /force

!ENDIF 

# Begin Target

# Name "HashMD5 - Win32 ReleaseWithTrace"
# Name "HashMD5 - Win32 Release"
# Name "HashMD5 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hash.c
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\hash_md5.c
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_hash_md5.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hash.h
# End Source File
# Begin Source File

SOURCE=..\HASH_API\hash_api.h
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\hash_md5.h
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\md5.h
# End Source File
# Begin Source File

SOURCE=.\plugin_hash_md5.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hash_md5.imp
# End Source File
# Begin Source File

SOURCE=.\hash_md5.meta
# End Source File
# Begin Source File

SOURCE=.\metadata.rc2
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\HashMD5.dep
# End Source File
# Begin Source File

SOURCE=.\HashMD5.mak
# End Source File
# End Target
# End Project

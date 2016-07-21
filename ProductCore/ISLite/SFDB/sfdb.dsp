# Microsoft Developer Studio Project File - Name="sfdb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=sfdb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sfdb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sfdb.mak" CFG="sfdb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sfdb - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sfdb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sfdb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/ISLite/sfdb", GELCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sfdb - Win32 ReleaseWithTrace"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/ReleaseWithTrace"
# PROP BASE Intermediate_Dir "../../../temp/ReleaseWithTrace/prague/ISLite/SFDB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/ReleaseWithTrace"
# PROP Intermediate_Dir "../../../temp/ReleaseWithTrace/prague/ISLite/SFDB"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /c
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
# ADD LINK32 /nologo /base:"0xAD900000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /out:"../../../out/ReleaseWithTrace/SFDB.PPL" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=\out\ReleaseWithTrace\SFDB.PPL
SOURCE="$(InputPath)"
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "sfdb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/ISLite/SFDB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/ISLite/SFDB"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /c
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
# ADD LINK32 user32.lib advapi32.lib /nologo /base:"0x67700000" /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Release/SFDB.PPL" /opt:ref
# SUBTRACT LINK32 /pdb:none /map /pdbtype:<none>
# Begin Special Build Tool
TargetPath=\out\Release\SFDB.PPL
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
PostBuild_Cmds=tsigner "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "sfdb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/ISLite/SFDB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/ISLite/SFDB"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../../out/Debug/SFDB.PPL" /implib:"" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "sfdb - Win32 ReleaseWithTrace"
# Name "sfdb - Win32 Release"
# Name "sfdb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\HASH_API\MD5\hash_md5.c
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin_sfdb.c
# End Source File
# Begin Source File

SOURCE=.\sa.cpp
# End Source File
# Begin Source File

SOURCE=.\sfdb.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\database.h
# End Source File
# Begin Source File

SOURCE=..\HASH_API\hash.h
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\hash_md5.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\iface\i_sfdb.h
# End Source File
# Begin Source File

SOURCE=.\locals.h
# End Source File
# Begin Source File

SOURCE=..\HASH_API\MD5\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\plugin\p_sfdb.h
# End Source File
# Begin Source File

SOURCE=.\plugin_sfdb.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\sa.h
# End Source File
# Begin Source File

SOURCE=.\sfdb.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\sfdb.meta
# End Source File
# End Group
# Begin Source File

SOURCE=.\sfdb.dep
# End Source File
# Begin Source File

SOURCE=.\sfdb.imp
# End Source File
# Begin Source File

SOURCE=.\sfdb.mak
# End Source File
# Begin Source File

SOURCE=..\..\Include\Prt\sfdb.prt
# End Source File
# End Target
# End Project

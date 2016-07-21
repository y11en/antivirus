# Microsoft Developer Studio Project File - Name="npr_ichecker2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=npr_ichecker2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "npr_ichecker2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "npr_ichecker2.mak" CFG="npr_ichecker2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "npr_ichecker2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "npr_ichecker2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Prague/ISLite/IChecker2", HBDEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "npr_ichecker2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../out/Release"
# PROP BASE Intermediate_Dir "../../../temp/Release/prague/ISLite/IChecker2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/prague/ISLite/IChecker2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NPR_ICHECKER2_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NPR_ICHECKER2_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "npr_ichecker2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../out/Debug"
# PROP BASE Intermediate_Dir "../../../temp/Debug/prague/ISLite/IChecker2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/prague/ISLite/IChecker2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NPR_ICHECKER2_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NPR_ICHECKER2_EXPORTS" /D "_NO_PRAGUE_" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../../out/Debug/nprichk2.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "npr_ichecker2 - Win32 Release"
# Name "npr_ichecker2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\calc_file_hash.c
# End Source File
# Begin Source File

SOURCE=..\..\hash\Hash_API\MD5\hash_md5.c
# End Source File
# Begin Source File

SOURCE=..\..\hash\Hash_API\MD5\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\npr_ichecker2.c
# End Source File
# Begin Source File

SOURCE=.\npr_io.c
# End Source File
# Begin Source File

SOURCE=..\sfdb\npr_sfdb.c
# End Source File
# Begin Source File

SOURCE=..\sfdb\npr_time.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\calc_file_hash.h
# End Source File
# Begin Source File

SOURCE=..\sfdb\database.h
# End Source File
# Begin Source File

SOURCE=.\ENTR_ELF.H
# End Source File
# Begin Source File

SOURCE=..\..\hash\Hash_API\hash_api.h
# End Source File
# Begin Source File

SOURCE=..\..\hash\Hash_API\MD5\hash_md5.h
# End Source File
# Begin Source File

SOURCE=.\itss.h
# End Source File
# Begin Source File

SOURCE=..\sfdb\locals.h
# End Source File
# Begin Source File

SOURCE=..\..\hash\Hash_API\MD5\md5.h
# End Source File
# Begin Source File

SOURCE=.\NEWEXE.H
# End Source File
# Begin Source File

SOURCE=.\npr.h
# End Source File
# Begin Source File

SOURCE=.\npr_hash.h
# End Source File
# Begin Source File

SOURCE=.\npr_ichecker2.h
# End Source File
# Begin Source File

SOURCE=.\npr_io.h
# End Source File
# Begin Source File

SOURCE=..\sfdb\npr_sfdb.h
# End Source File
# Begin Source File

SOURCE=..\sfdb\npr_time.h
# End Source File
# Begin Source File

SOURCE=.\ole2.h
# End Source File
# Begin Source File

SOURCE=.\PEHDR.H
# End Source File
# Begin Source File

SOURCE=.\rar.h
# End Source File
# Begin Source File

SOURCE=.\zip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

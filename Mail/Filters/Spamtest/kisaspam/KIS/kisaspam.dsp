# Microsoft Developer Studio Project File - Name="kisaspam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kisaspam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kisaspam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kisaspam.mak" CFG="kisaspam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kisaspam - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kisaspam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kisaspam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "kisaspam___Win32_Release"
# PROP BASE Intermediate_Dir "kisaspam___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "../tmp/kis/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../" /I "../_include" /I "../Filtration/API" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KISASPAM_EXPORTS" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /Gf /I "../" /I "../_include" /I "../Filtration/API" /D "NDEBUG" /D "FILTERDLL_IMPORT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KISASPAM_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libdb1_dmr.lib spamtest.lib pcommon_dmr.lib libnet_DMR.lib cfilter.lib libpatricia_dmr.lib common_dmr.lib libmimeparser_dmr.lib regex_dmr.lib libgsg2_dmr.lib librules_dmr.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib /nologo /dll /machine:I386 /libpath:"../lib"
# ADD LINK32 user32.lib Ws2_32.lib advapi32.lib /nologo /dll /machine:I386 /IGNORE:4089
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kisaspam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "kisaspam___Win32_Debug"
# PROP BASE Intermediate_Dir "kisaspam___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "../tmp/kis/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../" /I "../" /I "../_include" /I "../Filtration/API" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KISASPAM_EXPORTS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../" /I "../" /I "../_include" /I "../Filtration/API" /D "_DEBUG" /D "FILTERDLL_IMPORT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KISASPAM_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"kisaspamd.dll" /pdbtype:sept
# ADD LINK32 user32.lib Ws2_32.lib advapi32.lib /nologo /dll /debug /machine:I386 /out:"Debug/kisaspamd.dll" /pdbtype:sept /libpath:"../STL/LIB" /IGNORE:4089
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kisaspam - Win32 Release"
# Name "kisaspam - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageInfo\MessageInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SpamFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageInfo\stream.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\DataBase.h
# End Source File
# Begin Source File

SOURCE=.\KIS_FACE.H
# End Source File
# Begin Source File

SOURCE=.\MessageInfo\MessageInfo.h
# End Source File
# Begin Source File

SOURCE=.\SpamFilter.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "ext"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\client-filter\librules\actions.h"
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\adc\adc.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\BaseTsd.h"
# End Source File
# Begin Source File

SOURCE=..\Filtration\API\categories.h
# End Source File
# Begin Source File

SOURCE=..\gnu\libnet\cdefs.h
# End Source File
# Begin Source File

SOURCE=..\gnu\regex\sys\cdefs.h
# End Source File
# Begin Source File

SOURCE=..\Filtration\API\cfdata.h
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\common\config.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\include\Critsect.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\librules\envelope.h"
# End Source File
# Begin Source File

SOURCE=..\_include\export.h
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\libgsg2\gsg.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\Guiddef.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\librules\interpreter.h"
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\common\ip_extractor.h"
# End Source File
# Begin Source File

SOURCE=..\_include\language.h
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\libsigs\libsigs.h"
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\common\list_db.h"
# End Source File
# Begin Source File

SOURCE=..\gnu\md5a\md5a.h
# End Source File
# Begin Source File

SOURCE="..\smtp-filter\common\merge.h"
# End Source File
# Begin Source File

SOURCE=..\Filtration\API\message.h
# End Source File
# Begin Source File

SOURCE=..\gnu\libnet\nameser.h
# End Source File
# Begin Source File

SOURCE=..\gnu\libnet\nameser_compat.h
# End Source File
# Begin Source File

SOURCE=..\_include\nix_types.h
# End Source File
# Begin Source File

SOURCE="..\client-filter\librules\profile.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\librules\profile_db.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\librules\profile_db_impl.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\include\Progress.h"
# End Source File
# Begin Source File

SOURCE="..\client-filter\include\progress.rh"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\PropIdl.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\Reason.h"
# End Source File
# Begin Source File

SOURCE=..\gnu\regex\regex.h
# End Source File
# Begin Source File

SOURCE=..\Filtration\API\relevant.h
# End Source File
# Begin Source File

SOURCE="..\client-filter\include\SpamFilter.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\StrAlign.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\Tvout.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\Include\WinEFS.h"
# End Source File
# Begin Source File

SOURCE=..\_include\wintypes.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Target
# End Project

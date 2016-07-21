# Microsoft Developer Studio Project File - Name="conn_prod" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=conn_prod - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "conn_prod.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "conn_prod.mak" CFG="conn_prod - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "conn_prod - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "conn_prod - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "conn_prod"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "conn_prod - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONN_PROD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONN_PROD_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 klcsstd.lib klcskca.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /libpath:"../../lib/release"

!ELSEIF  "$(CFG)" == "conn_prod - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONN_PROD_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CONN_PROD_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 klcsstd.lib klcskca.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /libpath:"../../lib/debug"

!ENDIF 

# Begin Target

# Name "conn_prod - Win32 Release"
# Name "conn_prod - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\appguicalls.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\appsynchronizer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\connectorimp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\policysyncimp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\settingssyncimp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\tasksettingssynchronizerimp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\taskssynchronizerimp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\appguicalls.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\appsynchronizer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\connectorimp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\helpers.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\info.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\os_win32_4_0.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\plugin_info.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\policysyncimp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\settingssyncimp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\tasksettingssynchronizerimp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\testproducts\conn_prod\taskssynchronizerimp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

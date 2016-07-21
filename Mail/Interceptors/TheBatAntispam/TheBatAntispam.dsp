# Microsoft Developer Studio Project File - Name="TheBatAntispam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TheBatAntispam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TheBatAntispam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TheBatAntispam.mak" CFG="TheBatAntispam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TheBatAntispam - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TheBatAntispam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Interceptors/TheBatAntispam", OBMFBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TheBatAntispam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../out/Release"
# PROP Intermediate_Dir "../../../../temp/Release/ppp/batantispam"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../../../../prague/include" /I "../../../include" /I "../../../CSShare" /I "..\..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "THEBATANTISPAM_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:"../../../../out/Release/kav60as.pdb" /debug /machine:I386 /def:".\TheBatAntispam.def" /out:"../../../../out/Release/kav60.tbp" /libpath:"../../../../out/Release" /OPT:NOWIN98 /RELEASE /opt:ref
# SUBTRACT LINK32 /pdb:none /map

!ELSEIF  "$(CFG)" == "TheBatAntispam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../out/Debug"
# PROP Intermediate_Dir "../../../../temp/Debug/ppp/batantispam"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../prague/include" /I "../../../include" /I "../../../CSShare" /I "..\..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "THEBATANTISPAM_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /pdb:"../../../../out/Debug/kav60as.pdb" /debug /machine:I386 /out:"../../../../out/Debug/kav60.tbp" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TheBatAntispam - Win32 Release"
# Name "TheBatAntispam - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CheckerThread.cpp
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\CriticalSection.cpp
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\PragueHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\Register.cpp
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\remote_mbl.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TheBatAntispam.cpp
# End Source File
# Begin Source File

SOURCE=.\TheBatAntispam.def

!IF  "$(CFG)" == "TheBatAntispam - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "TheBatAntispam - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\Tracer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CheckerThread.h
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\CriticalSection.h
# End Source File
# Begin Source File

SOURCE=.\EnsureCleanup.h
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\PragueHelper.h
# End Source File
# Begin Source File

SOURCE=.\Register.h
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\remote_mbl.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TheBatAntispam.h
# End Source File
# Begin Source File

SOURCE=..\TheBatPlugin\Tracer.h
# End Source File
# Begin Source File

SOURCE=.\ver_mod.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

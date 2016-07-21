# Microsoft Developer Studio Project File - Name="fpirun" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=fpirun - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "fpirun.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fpirun.mak" CFG="fpirun - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fpirun - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "fpirun - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Anti-Virus/Tools/fpirun", ZWVAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "fpirun - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /Zi /I "c:\projects\anti-virus\f-prot\win32" /D "NDEBUG" /D "_AFXDLL" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ENABLE_MEASURE_STACK" /D "USE_FM" /YX /FD /c
# ADD BASE RSC /l 0x40b /d "NDEBUG"
# ADD RSC /l 0x40b /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 advapi32.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"../../out/release/fpirun.exe"

!ELSEIF  "$(CFG)" == "fpirun - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "c:\projects\anti-virus\f-prot\win32" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ENABLE_MEASURE_STACK" /D "USE_FM" /YX /FD /c
# ADD BASE RSC /l 0x40b /d "_DEBUG"
# ADD RSC /l 0x40b /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../out/debug/fpirun.exe"

!ENDIF 

# Begin Target

# Name "fpirun - Win32 Release"
# Name "fpirun - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\collect.cpp
# End Source File
# Begin Source File

SOURCE=.\filecallback.cpp
# End Source File
# Begin Source File

SOURCE=.\fpirun.cpp
# End Source File
# Begin Source File

SOURCE=.\mydir.c
# End Source File
# Begin Source File

SOURCE=.\reportcallback.cpp
# End Source File
# Begin Source File

SOURCE=.\RGSTRY32.CPP
# End Source File
# Begin Source File

SOURCE=.\statist.cpp
# End Source File
# Begin Source File

SOURCE=.\USCANMAN.CPP
# End Source File
# Begin Source File

SOURCE=.\fm4av.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\fileio.h
# End Source File
# Begin Source File

SOURCE=.\fmngm.h
# End Source File
# Begin Source File

SOURCE=.\fmtypes.h
# End Source File
# Begin Source File

SOURCE=.\fpiapi.h
# End Source File
# Begin Source File

SOURCE=.\fpirun.h
# End Source File
# Begin Source File

SOURCE=.\mydir.h
# End Source File
# Begin Source File

SOURCE=.\RGSTRY32.h
# End Source File
# Begin Source File

SOURCE=.\stdfm.h
# End Source File
# Begin Source File

SOURCE=.\uscanman.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\project.lnt
# End Source File
# End Target
# End Project

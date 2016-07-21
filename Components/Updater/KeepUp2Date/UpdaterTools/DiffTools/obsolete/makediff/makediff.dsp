# Microsoft Developer Studio Project File - Name="makediff" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=makediff - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "makediff.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "makediff.mak" CFG="makediff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "makediff - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "makediff - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Components/KeepUp2Date/DiffTools/makediff", TGIEAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "makediff - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /Gd /W3 /GX /Zi /O2 /Ob1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_WIN32_API" /FR /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../Release/makediff.exe" /fixed:no
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "makediff - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_WIN32_API" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../Debug/makediff.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "makediff - Win32 Release"
# Name "makediff - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\shared\common.c
# End Source File
# Begin Source File

SOURCE=..\shared\crc32.c
# End Source File
# Begin Source File

SOURCE=..\shared\diff.c
# End Source File
# Begin Source File

SOURCE=..\shared\diffunp.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=..\shared\prepbase.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\shared\common.h
# End Source File
# Begin Source File

SOURCE=..\shared\crc32.h
# End Source File
# Begin Source File

SOURCE=..\shared\diff.h
# End Source File
# Begin Source File

SOURCE=..\shared\differr.h
# End Source File
# Begin Source File

SOURCE=..\shared\prepbase.h
# End Source File
# Begin Source File

SOURCE=..\shared\types.h
# End Source File
# End Group
# Begin Group "ucl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\ucl\ucl.c
# End Source File
# Begin Source File

SOURCE=..\ucl\ucl.h
# End Source File
# Begin Source File

SOURCE=..\ucl\ucl_mchw.c
# End Source File
# Begin Source File

SOURCE=..\ucl\ucl_swd.c
# End Source File
# Begin Source File

SOURCE=..\ucl\uclunp.c
# End Source File
# End Group
# Begin Group "avepack"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\avepack\AvcPacker.c
# End Source File
# Begin Source File

SOURCE=..\avepack\AvcPacker.h
# End Source File
# Begin Source File

SOURCE=..\avepack\Base.h
# End Source File
# Begin Source File

SOURCE=..\avepack\CalcSum.c
# End Source File
# Begin Source File

SOURCE=..\avepack\CalcSum.h
# End Source File
# Begin Source File

SOURCE=..\avepack\fakeoop.c
# End Source File
# Begin Source File

SOURCE=..\avepack\fakeoop.h
# End Source File
# Begin Source File

SOURCE=..\avepack\Sq_s.c
# End Source File
# Begin Source File

SOURCE=..\avepack\Sq_su.h
# End Source File
# Begin Source File

SOURCE=..\avepack\Sq_u.c
# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib114.lib
# End Source File
# End Group
# End Target
# End Project

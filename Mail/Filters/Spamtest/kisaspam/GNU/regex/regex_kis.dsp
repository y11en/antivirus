# Microsoft Developer Studio Project File - Name="regex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=regex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "regex_kis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "regex_kis.mak" CFG="regex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "regex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Filters/Spamtest", EVPXBAAA"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "regex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "regex___Win32_Release"
# PROP BASE Intermediate_Dir "regex___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../kis/lib"
# PROP Intermediate_Dir "../../tmp/kis/Release/regex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Zi /O2 /I "../.." /I "." /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /FD /c
# SUBTRACT BASE CPP /X /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /O1 /I "../.." /I "." /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /Fd"../../tmp/kis/pdb/regex.pdb" /FD /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../lib/regex_dmr.lib"
# ADD LIB32 /nologo /out:"../../kis/lib/regex.lib"

!ELSEIF  "$(CFG)" == "regex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "regex___Win32_Debug"
# PROP BASE Intermediate_Dir "regex___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../kis/libd"
# PROP Intermediate_Dir "../../tmp/kis/Debug/regex"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /ZI /Od /I "../.." /I "." /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /ZI /Od /I "../.." /I "." /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /Fd"../../tmp/kis/pdbd/regex.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../lib/regex_dmd.lib"
# ADD LIB32 /nologo /out:"../../kis/libd/regex.lib"

!ENDIF 

# Begin Target

# Name "regex - Win32 Release"
# Name "regex - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\engine.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\regcomp.c
# End Source File
# Begin Source File

SOURCE=.\regerror.c
# End Source File
# Begin Source File

SOURCE=.\regexec.c
# End Source File
# Begin Source File

SOURCE=.\regfree.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cclass.h
# End Source File
# Begin Source File

SOURCE=.\sys\cdefs.h
# End Source File
# Begin Source File

SOURCE=.\cname.h
# End Source File
# Begin Source File

SOURCE=.\regex.h
# End Source File
# Begin Source File

SOURCE=.\regex2.h
# End Source File
# Begin Source File

SOURCE=.\regexp.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# End Group
# End Target
# End Project

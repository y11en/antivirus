# Microsoft Developer Studio Project File - Name="regex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=regex - Win32 Release_DMR
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "regex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "regex.mak" CFG="regex - Win32 Release_DMR"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "regex - Win32 Debug_SSD" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Release_SSR" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE "regex - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "regex - Win32 Debug_SSD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SSD"
# PROP BASE Intermediate_Dir "Debug_SSD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/regex/Debug_SSD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /ZI /Od /I "../.." /I "." /D "_DEBUG" /D "_ANSI_SOURCE" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/regex_ssd.lib"

!ELSEIF  "$(CFG)" == "regex - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SMD"
# PROP BASE Intermediate_Dir "Debug_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/regex/Debug_SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /ZI /Od /I "../.." /I "." /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/regex_smd.lib"

!ELSEIF  "$(CFG)" == "regex - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_DMD"
# PROP BASE Intermediate_Dir "Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/regex/Debug_DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /ZI /Od /I "../.." /I "." /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/regex_dmd.lib"

!ELSEIF  "$(CFG)" == "regex - Win32 Release_SSR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release_SSR"
# PROP BASE Intermediate_Dir "Release_SSR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/regex/Release_SSR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /ML /W3 /Zi /O2 /I "../.." /I "." /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /X /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/regex_ssr.lib"

!ELSEIF  "$(CFG)" == "regex - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release_SMR"
# PROP BASE Intermediate_Dir "Release_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/regex/Release_SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Zi /O2 /I "../.." /I "." /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /X /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/regex_smr.lib"

!ELSEIF  "$(CFG)" == "regex - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release_DMR"
# PROP BASE Intermediate_Dir "Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/regex/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /Zi /O2 /I "../.." /I "." /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /X /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/regex_dmr.lib"

!ENDIF 

# Begin Target

# Name "regex - Win32 Debug_SSD"
# Name "regex - Win32 Debug_SMD"
# Name "regex - Win32 Debug_DMD"
# Name "regex - Win32 Release_SSR"
# Name "regex - Win32 Release_SMR"
# Name "regex - Win32 Release_DMR"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\engine.c
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

# Microsoft Developer Studio Project File - Name="pcommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pcommon - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pcommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcommon.mak" CFG="pcommon - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pcommon - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "pcommon - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "pcommon - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE "pcommon - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pcommon - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pcommon___Win32_Debug_SMD"
# PROP BASE Intermediate_Dir "pcommon___Win32_Debug_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".."
# PROP Intermediate_Dir "../../tmp/lib/pcommon/Debug_SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\pcommon_ssd.lib"
# ADD LIB32 /nologo /out:"..\pcommon_smd.lib"

!ELSEIF  "$(CFG)" == "pcommon - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pcommon___Win32_Release_SMR"
# PROP BASE Intermediate_Dir "pcommon___Win32_Release_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".."
# PROP Intermediate_Dir "../../tmp/lib/pcommon/Release_SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../.." /I "../../_include" /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\pcommon_ssr.lib"
# ADD LIB32 /nologo /out:"..\pcommon_smr.lib"

!ELSEIF  "$(CFG)" == "pcommon - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pcommon___Win32_Debug_DMD"
# PROP BASE Intermediate_Dir "pcommon___Win32_Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".."
# PROP Intermediate_Dir "../../tmp/lib/pcommon/Debug_DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "WIN32" /D "_LIB" /Fr /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\pcommon_smd.lib"
# ADD LIB32 /nologo /out:"..\pcommon_dmd.lib"

!ELSEIF  "$(CFG)" == "pcommon - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pcommon___Win32_Release_DMR"
# PROP BASE Intermediate_Dir "pcommon___Win32_Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".."
# PROP Intermediate_Dir "../../tmp/lib/pcommon/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../.." /I "../../_include" /D "NDEBUG" /D "WIN32" /D "_LIB" /Fr /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\pcommon_smr.lib"
# ADD LIB32 /nologo /out:"..\pcommon_dmr.lib"

!ENDIF 

# Begin Target

# Name "pcommon - Win32 Debug_SMD"
# Name "pcommon - Win32 Release_SMR"
# Name "pcommon - Win32 Debug_DMD"
# Name "pcommon - Win32 Release_DMR"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\convert.cpp
# End Source File
# Begin Source File

SOURCE=.\Dos2Uni.cpp
# End Source File
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\Iso2Uni.cpp
# End Source File
# Begin Source File

SOURCE=.\Koi2Uni.cpp
# End Source File
# Begin Source File

SOURCE=.\koi8_ctype.c
# End Source File
# Begin Source File

SOURCE=.\Lat12Uni.cpp
# End Source File
# Begin Source File

SOURCE=.\Lat12Uni_cyr.cpp
# End Source File
# Begin Source File

SOURCE=.\Mac2Uni.cpp
# End Source File
# Begin Source File

SOURCE=.\nix_types.c
# End Source File
# Begin Source File

SOURCE=.\strptime.c
# End Source File
# Begin Source File

SOURCE=.\Win2Uni.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\_include\_time.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\_unistd.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\CodePages.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\koi8_ctype.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\language.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\nix_types.h
# End Source File
# Begin Source File

SOURCE=..\pcommon.link.h
# End Source File
# Begin Source File

SOURCE=.\StdPch.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\wintypes.h
# End Source File
# End Group
# Begin Group "STL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_config.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_config_compat.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_config_compat_post.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\_epilog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_epilog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\_msvc_warnings_off.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl\_site_config.h
# End Source File
# Begin Source File

SOURCE="..\..\..\..\!Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\..\_include\Cp2Uni.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stl_confix.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stl_msvc.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stl_select_lib.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\stl_user_config.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\stlcomp.h
# End Source File
# Begin Source File

SOURCE=..\..\STL\stlport\config\vc_select_lib.h
# End Source File
# End Group
# Begin Group "system"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\BaseTsd.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\Guiddef.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\PropIdl.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\Reason.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\StrAlign.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\Tvout.h"
# End Source File
# Begin Source File

SOURCE="C:\!Studio6\SDK\Include\WinEFS.h"
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="cspwdlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cspwdlib - Win32 Debug MBCS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cspwdlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cspwdlib.mak" CFG="cspwdlib - Win32 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cspwdlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cspwdlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "cspwdlib - Win32 Debug MBCS" (based on "Win32 (x86) Static Library")
!MESSAGE "cspwdlib - Win32 Release MBCS" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CS AdminKit/development2/std/klcspwd/cspwdlib", SKECCAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cspwdlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../lib/release"
# PROP Intermediate_Dir "../../../bin/release/std/cspwdlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /Ob2 /I "../../../openssl/include" /I "../../../include" /I "../../../../../commonfiles/stuff" /I "../../../../../commonfiles" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "UNICODE" /D "KLSTD_WINNT" /D "_LIB" /D "KLCS_USE_CSPWDLIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../../commonfiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cspwdlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib/debug"
# PROP Intermediate_Dir "../../../bin/debug/std/cspwdlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../openssl/include" /I "../../../include" /I "../../../../../commonfiles/stuff" /I "../../../../../commonfiles" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "UNICODE" /D "KLSTD_WINNT" /D "_LIB" /D "KLCS_USE_CSPWDLIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../../commonfiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cspwdlib - Win32 Debug MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug MBCS"
# PROP BASE Intermediate_Dir "Debug MBCS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib/debug_mbcs"
# PROP Intermediate_Dir "../../../bin/debug_mbcs/std/cspwdlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../openssl/include" /I "../../../include" /I "../../../../../commonfiles/stuff" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "UNICODE" /D "KLSTD_WINNT" /D "_LIB" /D "KLCS_USE_CSPWDLIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../openssl/include" /I "../../../include" /I "../../../../../commonfiles/stuff" /I "../../../../../commonfiles" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "KLSTD_WIN9X" /D "_LIB" /D "KLCS_USE_CSPWDLIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /i "../../../../../commonfiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cspwdlib - Win32 Release MBCS"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release MBCS"
# PROP BASE Intermediate_Dir "Release MBCS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../lib/release_mbcs"
# PROP Intermediate_Dir "../../../bin/release_mbcs/std/cspwdlib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /O1 /Ob2 /I "../../../openssl/include" /I "../../../include" /I "../../../../../commonfiles/stuff" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "UNICODE" /D "KLSTD_WINNT" /D "_LIB" /D "KLCS_USE_CSPWDLIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /Ob2 /I "../../../openssl/include" /I "../../../include" /I "../../../../../commonfiles/stuff" /I "../../../../../commonfiles" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "KLSTD_WIN9X" /D "_LIB" /D "KLCS_USE_CSPWDLIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /i "../../../../../commonfiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "cspwdlib - Win32 Release"
# Name "cspwdlib - Win32 Debug"
# Name "cspwdlib - Win32 Debug MBCS"
# Name "cspwdlib - Win32 Release MBCS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\conv.h
# End Source File
# Begin Source File

SOURCE=..\crypto.cpp
# End Source File
# Begin Source File

SOURCE=..\klcspwd.cpp
# End Source File
# Begin Source File

SOURCE=..\klcspwd_inst.cpp
# End Source File
# Begin Source File

SOURCE=..\strg_9x.cpp
# End Source File
# Begin Source File

SOURCE=..\strg_nt.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\crypto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\errorcodes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\klcspwd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\klcspwd_common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\klcspwd_inst.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\klcspwdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\std\klcspwd\prtstrg.h
# End Source File
# End Group
# End Target
# End Project

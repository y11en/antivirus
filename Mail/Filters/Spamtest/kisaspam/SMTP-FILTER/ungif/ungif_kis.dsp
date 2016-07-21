# Microsoft Developer Studio Project File - Name="ungif" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ungif - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ungif_kis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ungif_kis.mak" CFG="ungif - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ungif - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ungif - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ungif - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ungif___Win32_Release"
# PROP BASE Intermediate_Dir "ungif___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../kis/lib"
# PROP Intermediate_Dir "../../tmp/kis/Release/ungif"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../.." /D "NDEBUG" /D "WIN32" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../.." /D "NDEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /D "WITHOUT_FILE" /Fd"../../tmp/kis/pdb/ungif.pdb" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"ungif_dmr.lib"
# ADD LIB32 /nologo /out:"../../kis/lib/ungif.lib"

!ELSEIF  "$(CFG)" == "ungif - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ungif___Win32_Debug"
# PROP BASE Intermediate_Dir "ungif___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../kis/libd"
# PROP Intermediate_Dir "../../tmp/kis/Debug/ungif"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "WIN32" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /D "WITHOUT_FILE" /Fd"../../tmp/kis/pdbd/ungif.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"ungif_dmd.lib"
# ADD LIB32 /nologo /out:"../../kis/libd/ungif.lib"

!ENDIF 

# Begin Target

# Name "ungif - Win32 Release"
# Name "ungif - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lib\dgif_lib.c
# End Source File
# Begin Source File

SOURCE=.\lib\egif_lib.c
# End Source File
# Begin Source File

SOURCE=.\lib\gif_err.c
# End Source File
# Begin Source File

SOURCE=.\lib\gifalloc.c
# End Source File
# Begin Source File

SOURCE=.\lib\qprintf.c
# End Source File
# Begin Source File

SOURCE=.\lib\quantize.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lib\gif_lib.h
# End Source File
# Begin Source File

SOURCE=.\lib\gif_lib_private.h
# End Source File
# End Group
# End Target
# End Project

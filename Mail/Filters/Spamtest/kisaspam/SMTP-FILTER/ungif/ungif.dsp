# Microsoft Developer Studio Project File - Name="ungif" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ungif - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ungif.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ungif.mak" CFG="ungif - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ungif - Win32 Debug_SSD" (based on "Win32 (x86) Static Library")
!MESSAGE "ungif - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "ungif - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "ungif - Win32 Release_SSR" (based on "Win32 (x86) Static Library")
!MESSAGE "ungif - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE "ungif - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ungif - Win32 Debug_SSD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SSD"
# PROP BASE Intermediate_Dir "Debug_SSD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/ungif/Debug_SSD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../.." /D "_DEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ungif_ssd.lib"

!ELSEIF  "$(CFG)" == "ungif - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SMD"
# PROP BASE Intermediate_Dir "Debug_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/ungif/Debug_SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../.." /D "_DEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ungif_smd.lib"

!ELSEIF  "$(CFG)" == "ungif - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_DMD"
# PROP BASE Intermediate_Dir "Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/ungif/Debug_DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ungif_dmd.lib"

!ELSEIF  "$(CFG)" == "ungif - Win32 Release_SSR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_SSR"
# PROP BASE Intermediate_Dir "Release_SSR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/ungif/Release_SSR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../.." /D "NDEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ungif_ssr.lib"

!ELSEIF  "$(CFG)" == "ungif - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_SMR"
# PROP BASE Intermediate_Dir "Release_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/ungif/Release_SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../.." /D "NDEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ungif_smr.lib"

!ELSEIF  "$(CFG)" == "ungif - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_DMR"
# PROP BASE Intermediate_Dir "Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/ungif/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../.." /D "NDEBUG" /D "WIN32" /D "_LIB" /D "HAVE_FCNTL_H" /D "HAVE_STDARG_H" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ungif_dmr.lib"

!ENDIF 

# Begin Target

# Name "ungif - Win32 Debug_SSD"
# Name "ungif - Win32 Debug_SMD"
# Name "ungif - Win32 Debug_DMD"
# Name "ungif - Win32 Release_SSR"
# Name "ungif - Win32 Release_SMR"
# Name "ungif - Win32 Release_DMR"
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

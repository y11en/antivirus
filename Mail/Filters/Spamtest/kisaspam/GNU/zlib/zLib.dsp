# Microsoft Developer Studio Project File - Name="zLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zLib - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zLib.mak" CFG="zLib - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zLib - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "zLib - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "zLib - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE "zLib - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zLib - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zLib___Win32_Debug_DMD"
# PROP BASE Intermediate_Dir "zLib___Win32_Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/zlib/Debug_DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Lib/libZ_dmd.lib"
# ADD LIB32 /nologo /out:"../Lib/libZ_dmd.lib"

!ELSEIF  "$(CFG)" == "zLib - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zLib___Win32_Debug_SMD"
# PROP BASE Intermediate_Dir "zLib___Win32_Debug_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/zlib/Debug_SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Lib/libZ_smd.lib"
# ADD LIB32 /nologo /out:"../Lib/libZ_smd.lib"

!ELSEIF  "$(CFG)" == "zLib - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zLib___Win32_Release_DMR"
# PROP BASE Intermediate_Dir "zLib___Win32_Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/zlib/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Lib/libZ_dmr.lib"
# ADD LIB32 /nologo /out:"../Lib/libZ_dmr.lib"

!ELSEIF  "$(CFG)" == "zLib - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zLib___Win32_Release_SMR"
# PROP BASE Intermediate_Dir "zLib___Win32_Release_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "../../tmp/gnu/zlib/Release_SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Lib/libZ_smr.lib"
# ADD LIB32 /nologo /out:"../Lib/libZ_smr.lib"

!ENDIF 

# Begin Target

# Name "zLib - Win32 Debug_DMD"
# Name "zLib - Win32 Debug_SMD"
# Name "zLib - Win32 Release_DMR"
# Name "zLib - Win32 Release_SMR"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\adler32.c
# End Source File
# Begin Source File

SOURCE=.\compress.c
# End Source File
# Begin Source File

SOURCE=.\crc32.c
# End Source File
# Begin Source File

SOURCE=.\deflate.c
# End Source File
# Begin Source File

SOURCE=.\gzio.c
# End Source File
# Begin Source File

SOURCE=.\infblock.c
# End Source File
# Begin Source File

SOURCE=.\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\inffast.c
# End Source File
# Begin Source File

SOURCE=.\inflate.c
# End Source File
# Begin Source File

SOURCE=.\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\infutil.c
# End Source File
# Begin Source File

SOURCE=.\maketree.c
# End Source File
# Begin Source File

SOURCE=.\trees.c
# End Source File
# Begin Source File

SOURCE=.\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\zutil.c
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\deflate.h
# End Source File
# Begin Source File

SOURCE=.\infblock.h
# End Source File
# Begin Source File

SOURCE=.\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\inffast.h
# End Source File
# Begin Source File

SOURCE=.\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\infutil.h
# End Source File
# Begin Source File

SOURCE=.\trees.h
# End Source File
# Begin Source File

SOURCE=.\zconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib.h
# End Source File
# Begin Source File

SOURCE=..\zlib.link.h
# End Source File
# Begin Source File

SOURCE=.\zutil.h
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
# End Target
# End Project

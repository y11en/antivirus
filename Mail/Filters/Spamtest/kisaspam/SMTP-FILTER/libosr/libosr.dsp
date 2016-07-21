# Microsoft Developer Studio Project File - Name="libosr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libosr - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libosr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libosr.mak" CFG="libosr - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libosr - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libosr - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libosr - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_DMD"
# PROP BASE Intermediate_Dir "Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libosr/Debug_DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../gdwin32" /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "_DEBUG" /D "WITH_DUMP" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\libosr_dmd.lib"

!ELSEIF  "$(CFG)" == "libosr - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_DMR"
# PROP BASE Intermediate_Dir "Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libosr/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "../gdwin32" /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "NDEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\libosr_dmr.lib"

!ENDIF 

# Begin Target

# Name "libosr - Win32 Debug_DMD"
# Name "libosr - Win32 Release_DMR"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dump_c.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_c.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_dump.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_error.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_expert.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_image.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_image_analyzer.cpp
# End Source File
# Begin Source File

SOURCE=.\osr_text_analyzer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\array.h
# End Source File
# Begin Source File

SOURCE=.\dump_c.h
# End Source File
# Begin Source File

SOURCE=.\osr_c.h
# End Source File
# Begin Source File

SOURCE=.\osr_common.h
# End Source File
# Begin Source File

SOURCE=.\osr_dump.h
# End Source File
# Begin Source File

SOURCE=.\osr_error.h
# End Source File
# Begin Source File

SOURCE=.\osr_expert.h
# End Source File
# Begin Source File

SOURCE=.\osr_image.h
# End Source File
# Begin Source File

SOURCE=.\osr_image_analyzer.h
# End Source File
# Begin Source File

SOURCE=.\osr_symbol.h
# End Source File
# Begin Source File

SOURCE=.\osr_text_analyzer.h
# End Source File
# Begin Source File

SOURCE=.\osr_version.h
# End Source File
# End Group
# Begin Group "format"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libgsg2\binary_handler1.c
# End Source File
# Begin Source File

SOURCE=..\libgsg2\gsg.h
# End Source File
# Begin Source File

SOURCE="..\libgsg2\image-gif.c"
# End Source File
# Begin Source File

SOURCE="..\libgsg2\image-jpeg.c"
# End Source File
# Begin Source File

SOURCE="..\libgsg2\image-png.c"
# End Source File
# Begin Source File

SOURCE=..\libgsg2\image.c
# End Source File
# End Group
# End Target
# End Project

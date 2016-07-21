# Microsoft Developer Studio Project File - Name="libosr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libosr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libosr_kis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libosr_kis.mak" CFG="libosr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libosr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libosr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libosr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../kis/libd"
# PROP Intermediate_Dir "../../tmp/kis/Debug/libosr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /I "../gdwin32" /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "_DEBUG" /D "WIN32" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../kis/libd/libosr.lib"

!ELSEIF  "$(CFG)" == "libosr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../kis/lib"
# PROP Intermediate_Dir "../../tmp/kis/Release/libosr"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "../gdwin32" /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "NDEBUG" /D "WIN32" /D "_LIB" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../kis/lib/libosr.lib"

!ENDIF 

# Begin Target

# Name "libosr - Win32 Debug"
# Name "libosr - Win32 Release"
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

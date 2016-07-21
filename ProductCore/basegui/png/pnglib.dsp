# Microsoft Developer Studio Project File - Name="pnglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pnglib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pnglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pnglib.mak" CFG="pnglib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pnglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pnglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/basegui/png", KJNHBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pnglib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libpng___Win32_LIB_Release"
# PROP BASE Intermediate_Dir "libpng___Win32_LIB_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../out/Release"
# PROP Intermediate_Dir "../../../temp/Release/ppp/pnglib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /D "WIN32" /D "NDEBUG" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /Zi /O1 /I "..\..\..\Prague\Include" /D "NDEBUG" /D "WIN32" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libpng___Win32_LIB_Debug"
# PROP BASE Intermediate_Dir "libpng___Win32_LIB_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../out/Debug"
# PROP Intermediate_Dir "../../../temp/Debug/ppp/pnglib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Prague\Include" /D "_DEBUG" /D "DEBUG" /D PNG_DEBUG=1 /D "WIN32" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "pnglib - Win32 Release"
# Name "pnglib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=png.c
# End Source File
# Begin Source File

SOURCE=pngerror.c
# End Source File
# Begin Source File

SOURCE=pngget.c
# End Source File
# Begin Source File

SOURCE=.\pngimg.cpp
# End Source File
# Begin Source File

SOURCE=pngmem.c
# End Source File
# Begin Source File

SOURCE=pngpread.c
# End Source File
# Begin Source File

SOURCE=pngread.c
# End Source File
# Begin Source File

SOURCE=pngrio.c
# End Source File
# Begin Source File

SOURCE=pngrtran.c
# End Source File
# Begin Source File

SOURCE=pngrutil.c
# End Source File
# Begin Source File

SOURCE=pngset.c
# End Source File
# Begin Source File

SOURCE=pngtrans.c
# End Source File
# Begin Source File

SOURCE=pngvcrd.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=pngwio.c
# End Source File
# Begin Source File

SOURCE=pngwrite.c
# End Source File
# Begin Source File

SOURCE=pngwtran.c
# End Source File
# Begin Source File

SOURCE=pngwutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=png.h
# End Source File
# Begin Source File

SOURCE=pngconf.h
# End Source File
# Begin Source File

SOURCE=.\pngimg.h
# End Source File
# End Group
# Begin Group "ZLib"

# PROP Default_Filter ""
# Begin Group "ZLib.c"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\zlib\inflate_e.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\zlib\zutil.c
# End Source File
# End Group
# Begin Group "ZLib.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\zlib\crc32.h
# End Source File
# Begin Source File

SOURCE=.\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inflate.h
# End Source File
# Begin Source File

SOURCE=.\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\zlib\zutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\zlib\algorithm.txt
# End Source File
# End Group
# Begin Source File

SOURCE=pnglib.txt
# End Source File
# End Target
# End Project

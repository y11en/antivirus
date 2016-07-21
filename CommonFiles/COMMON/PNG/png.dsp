# Microsoft Developer Studio Project File - Name="png" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=png - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "png.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "png.mak" CFG="png - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "png - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "png - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Release/Kaspersky Anti-Virus Personal 5.0 MP1/CommonFiles/COMMON/png", HUFKAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "png - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\temp\Release\png"
# PROP Intermediate_Dir "..\..\..\temp\Release\png"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\out\Release\png.lib"

!ELSEIF  "$(CFG)" == "png - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\temp\Debug\png"
# PROP Intermediate_Dir "..\..\..\temp\Debug\png"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MDd /W3 /GX /Z7 /Od /I "..\zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\out\Debug\png.lib"

!ENDIF 

# Begin Target

# Name "png - Win32 Release"
# Name "png - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Png.c
# End Source File
# Begin Source File

SOURCE=.\Pngerror.c
# End Source File
# Begin Source File

SOURCE=.\pngget.c
# End Source File
# Begin Source File

SOURCE=.\Pngmem.c
# End Source File
# Begin Source File

SOURCE=.\Pngpread.c
# End Source File
# Begin Source File

SOURCE=.\Pngread.c
# End Source File
# Begin Source File

SOURCE=.\Pngrio.c
# End Source File
# Begin Source File

SOURCE=.\Pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\Pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\pngset.c
# End Source File
# Begin Source File

SOURCE=.\Pngtest.c
# End Source File
# Begin Source File

SOURCE=.\Pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\Pngwio.c
# End Source File
# Begin Source File

SOURCE=.\Pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\Pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\Pngwutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\png.h
# End Source File
# Begin Source File

SOURCE=.\pngconf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

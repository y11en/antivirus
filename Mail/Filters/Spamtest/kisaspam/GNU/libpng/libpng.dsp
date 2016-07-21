# Microsoft Developer Studio Project File - Name="libpng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libpng - Win32 Debug_SSD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 Debug_SSD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug_SSD" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Release_SSR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libpng - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libpng___Win32_DMD"
# PROP BASE Intermediate_Dir "libpng___Win32_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\..\tmp\gnu\libpng\DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng_DMD.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libpng___Win32_SMD"
# PROP BASE Intermediate_Dir "libpng___Win32_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\..\tmp\gnu\libpng\SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng_SMD.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SSD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libpng___Win32_SSD"
# PROP BASE Intermediate_Dir "libpng___Win32_SSD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\..\tmp\gnu\libpng\SSD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng_SSD.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libpng___Win32_DMR"
# PROP BASE Intermediate_Dir "libpng___Win32_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\..\tmp\gnu\libpng\DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng_DMR.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libpng___Win32_SMR"
# PROP BASE Intermediate_Dir "libpng___Win32_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\..\tmp\gnu\libpng\SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng_SMR.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SSR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libpng___Win32_SSR"
# PROP BASE Intermediate_Dir "libpng___Win32_SSR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "..\..\tmp\gnu\libpng\SSR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng_SSR.lib"

!ENDIF 

# Begin Target

# Name "libpng - Win32 Debug_DMD"
# Name "libpng - Win32 Debug_SMD"
# Name "libpng - Win32 Debug_SSD"
# Name "libpng - Win32 Release_DMR"
# Name "libpng - Win32 Release_SMR"
# Name "libpng - Win32 Release_SSR"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\png.c
# End Source File
# Begin Source File

SOURCE=.\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\pngget.c
# End Source File
# Begin Source File

SOURCE=.\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\pngread.c
# End Source File
# Begin Source File

SOURCE=.\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\pngset.c
# End Source File
# Begin Source File

SOURCE=.\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\pngvcrd.c

!IF  "$(CFG)" == "libpng - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_DMR"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SSR"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\scripts\pngw32.def

!IF  "$(CFG)" == "libpng - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_DMR"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SSR"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\pngwutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\png.h
# End Source File
# Begin Source File

SOURCE=.\pngconf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\scripts\pngw32.rc

!IF  "$(CFG)" == "libpng - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_DMR"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libpng - Win32 Release_SSR"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="libgsg2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libgsg2 - Win32 Debug_DMD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libgsg2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgsg2.mak" CFG="libgsg2 - Win32 Debug_DMD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgsg2 - Win32 Debug_SSD" (based on "Win32 (x86) Static Library")
!MESSAGE "libgsg2 - Win32 Debug_SMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libgsg2 - Win32 Debug_DMD" (based on "Win32 (x86) Static Library")
!MESSAGE "libgsg2 - Win32 Release_SSR" (based on "Win32 (x86) Static Library")
!MESSAGE "libgsg2 - Win32 Release_SMR" (based on "Win32 (x86) Static Library")
!MESSAGE "libgsg2 - Win32 Release_DMR" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libgsg2 - Win32 Debug_SSD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SSD"
# PROP BASE Intermediate_Dir "Debug_SSD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libgsg2/Debug_SSD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "_DEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libgsg2_ssd.lib"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_SMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SMD"
# PROP BASE Intermediate_Dir "Debug_SMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libgsg2/Debug_SMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "_DEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libgsg2_smd.lib"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_DMD"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_DMD"
# PROP BASE Intermediate_Dir "Debug_DMD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libgsg2/Debug_DMD"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "_DEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libgsg2_dmd.lib"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SSR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_SSR"
# PROP BASE Intermediate_Dir "Release_SSR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libgsg2/Release_SSR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "NDEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libgsg2_ssr.lib"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_SMR"
# PROP BASE Intermediate_Dir "Release_SMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libgsg2/Release_SMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "NDEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libgsg2_smr.lib"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_DMR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_DMR"
# PROP BASE Intermediate_Dir "Release_DMR"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "../../tmp/smtp-filter/libgsg2/Release_DMR"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../.." /I "../../_include" /I "../../gnu/zlib" /D "NDEBUG" /D "WIN32" /D "_LIB" /D "WITH_DUMP" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"libgsg2_dmr.lib"

!ENDIF 

# Begin Target

# Name "libgsg2 - Win32 Debug_SSD"
# Name "libgsg2 - Win32 Debug_SMD"
# Name "libgsg2 - Win32 Debug_DMD"
# Name "libgsg2 - Win32 Release_SSR"
# Name "libgsg2 - Win32 Release_SMR"
# Name "libgsg2 - Win32 Release_DMR"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\binary_handler.c

!IF  "$(CFG)" == "libgsg2 - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SSR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_DMR"

# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\binary_handler1.c

!IF  "$(CFG)" == "libgsg2 - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SSR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_DMR"

# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\image-gif.c"

!IF  "$(CFG)" == "libgsg2 - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SSR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_DMR"

# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\image-jpeg.c"

!IF  "$(CFG)" == "libgsg2 - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SSR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_DMR"

# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\image-png.c"
# End Source File
# Begin Source File

SOURCE=.\image.c

!IF  "$(CFG)" == "libgsg2 - Win32 Debug_SSD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_SMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug_DMD"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SSR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_SMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Release_DMR"

# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\_include\_unistd.h
# End Source File
# Begin Source File

SOURCE=..\ungif\gif_lib.h
# End Source File
# Begin Source File

SOURCE=.\gsg.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=..\libsigs\libsigs.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu\md5a\md5a.h
# End Source File
# Begin Source File

SOURCE=..\..\_include\nix_types.h
# End Source File
# End Group
# End Target
# End Project

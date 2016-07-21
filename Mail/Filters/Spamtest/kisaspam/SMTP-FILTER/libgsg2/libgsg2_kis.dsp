# Microsoft Developer Studio Project File - Name="libgsg2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libgsg2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libgsg2_kis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libgsg2_kis.mak" CFG="libgsg2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libgsg2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libgsg2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PPP/MailCommon/Filters/Spamtest", EVPXBAAA"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libgsg2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libgsg2___Win32_Release"
# PROP BASE Intermediate_Dir "libgsg2___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../kis/lib"
# PROP Intermediate_Dir "../../tmp/kis/Release/libgsg2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../.." /I "../../_include" /D "NDEBUG" /D "WIN32" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /Fr /YX
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../.." /I "../../_include" /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /Fd"../../tmp/kis/pdb/libgsg2.pdb" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libgsg2_dmr.lib"
# ADD LIB32 /nologo /out:"../../kis/lib/libgsg2.lib"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libgsg2___Win32_Debug"
# PROP BASE Intermediate_Dir "libgsg2___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../kis/libd"
# PROP Intermediate_Dir "../../tmp/kis/Debug/libgsg2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "WIN32" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../.." /I "../../_include" /D "_DEBUG" /D "WIN32" /D "_LIB" /Fd"../../tmp/kis/pdbd/libgsg2.pdb" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"libgsg2_dmd.lib"
# ADD LIB32 /nologo /out:"../../kis/libd/libgsg2.lib"

!ENDIF 

# Begin Target

# Name "libgsg2 - Win32 Release"
# Name "libgsg2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\binary_handler.c

!IF  "$(CFG)" == "libgsg2 - Win32 Release"

# PROP BASE Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"
# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\binary_handler1.c

!IF  "$(CFG)" == "libgsg2 - Win32 Release"

# PROP BASE Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"
# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\image-gif.c"

!IF  "$(CFG)" == "libgsg2 - Win32 Release"

# PROP BASE Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"
# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\image-jpeg.c"

!IF  "$(CFG)" == "libgsg2 - Win32 Release"

# PROP BASE Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"
# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\image.c

!IF  "$(CFG)" == "libgsg2 - Win32 Release"

# PROP BASE Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"
# PROP Intermediate_Dir "../../tmp/lib/gsg2/Release_DMR"

!ELSEIF  "$(CFG)" == "libgsg2 - Win32 Debug"

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

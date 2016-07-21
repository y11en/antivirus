# Microsoft Developer Studio Project File - Name="dskm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dskm - Win32 DebugDll
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dskm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dskm.mak" CFG="dskm - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dskm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dskm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "dskm - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "dskm - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/dskm", PFNPBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dskm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/dskm"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "dskm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/dskm"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "dskm - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dskm___Win32_DebugDll"
# PROP BASE Intermediate_Dir "dskm___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/dskm"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "dskm - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dskm___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "dskm___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/dskm"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Zi /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "dskm - Win32 Release"
# Name "dskm - Win32 Debug"
# Name "dskm - Win32 DebugDll"
# Name "dskm - Win32 ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cl_check.c
# End Source File
# Begin Source File

SOURCE=.\cl_gkey.c
# End Source File
# Begin Source File

SOURCE=.\cl_hash.c
# End Source File
# Begin Source File

SOURCE=.\cl_hass.c
# End Source File
# Begin Source File

SOURCE=.\cl_init.c
# End Source File
# Begin Source File

SOURCE=.\cl_mkey.c
# End Source File
# Begin Source File

SOURCE=.\cl_sign.c
# End Source File
# Begin Source File

SOURCE=.\cl_util.c
# End Source File
# Begin Source File

SOURCE=.\ds_assch.c
# End Source File
# Begin Source File

SOURCE=.\ds_assoc.c
# End Source File
# Begin Source File

SOURCE=.\ds_check.c
# End Source File
# Begin Source File

SOURCE=.\ds_chkas.c
# End Source File
# Begin Source File

SOURCE=.\ds_chkbf.c
# End Source File
# Begin Source File

SOURCE=.\ds_chkfd.c
# End Source File
# Begin Source File

SOURCE=.\ds_chkfl.c
# End Source File
# Begin Source File

SOURCE=.\ds_compo.c
# End Source File
# Begin Source File

SOURCE=.\ds_cregs.c
# End Source File
# Begin Source File

SOURCE=.\ds_deser.c
# End Source File
# Begin Source File

SOURCE=.\ds_dump.c
# End Source File
# Begin Source File

SOURCE=.\ds_erro.c
# End Source File
# Begin Source File

SOURCE=.\ds_excpt.c
# End Source File
# Begin Source File

SOURCE=.\ds_ff.c
# End Source File
# Begin Source File

SOURCE=.\ds_fkey.c
# End Source File
# Begin Source File

SOURCE=.\ds_gkey.c
# End Source File
# Begin Source File

SOURCE=.\ds_hash.c
# End Source File
# Begin Source File

SOURCE=.\ds_init.c
# End Source File
# Begin Source File

SOURCE=.\ds_inite.c
# End Source File
# Begin Source File

SOURCE=.\ds_io.c
# End Source File
# Begin Source File

SOURCE=.\ds_list.c
# End Source File
# Begin Source File

SOURCE=.\ds_objsb.c
# End Source File
# Begin Source File

SOURCE=.\ds_objse.c
# End Source File
# Begin Source File

SOURCE=.\ds_objsn.c
# End Source File
# Begin Source File

SOURCE=.\ds_objsp.c
# End Source File
# Begin Source File

SOURCE=.\ds_pars.c
# End Source File
# Begin Source File

SOURCE=.\ds_parso.c
# End Source File
# Begin Source File

SOURCE=.\ds_preg.c
# End Source File
# Begin Source File

SOURCE=.\ds_serial.c
# End Source File
# Begin Source File

SOURCE=.\ds_utils.c
# End Source File
# Begin Source File

SOURCE=..\Stuff\TEXT2BIN.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cl_util.h
# End Source File
# Begin Source File

SOURCE=.\dskm.h
# End Source File
# Begin Source File

SOURCE=.\dskmdefs.h
# End Source File
# Begin Source File

SOURCE=.\dskmi.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\GOST\Debug\CryptoC.lib
# End Source File
# End Target
# End Project

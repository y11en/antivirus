# Microsoft Developer Studio Project File - Name="Property" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Property - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Property.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Property.mak" CFG="Property - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Property - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Property""
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Property - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Property___Win32_DebugDll"
# PROP BASE Intermediate_Dir "Property___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\DebugS\Property.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Property___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "Property___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\ReleaseS\Property.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Property___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "Property___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugU"
# PROP Intermediate_Dir "../../Temp/DebugU/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "_DEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Debug\Property.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugUDll
InputPath=\Out\DebugU\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Property___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "Property___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../Temp/ReleaseUS/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "NDEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\ReleaseS\Property.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseU
InputPath=\Out\ReleaseUS\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Property___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "Property___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugUS"
# PROP Intermediate_Dir "../../Temp/DebugUS/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "_DEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\DebugS\Property.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugU
InputPath=\Out\DebugUS\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugU

# End Custom Build

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Property___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "Property___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseU"
# PROP Intermediate_Dir "../../Temp/ReleaseU/CommonFiles/Property"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\\" /D "NDEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Release\Property.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseUDll
InputPath=\Out\ReleaseU\Property.lib
InputName=Property
SOURCE="$(InputPath)"

"..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseUDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "Property - Win32 Release"
# Name "Property - Win32 Debug"
# Name "Property - Win32 DebugDll"
# Name "Property - Win32 ReleaseDll"
# Name "Property - Win32 Unicode DebugDll"
# Name "Property - Win32 Unicode Release"
# Name "Property - Win32 Unicode Debug"
# Name "Property - Win32 Unicode ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CRC.C
# End Source File
# Begin Source File

SOURCE=.\d_add.c
# End Source File
# Begin Source File

SOURCE=.\d_add_pr.c
# End Source File
# Begin Source File

SOURCE=.\d_attach.c
# End Source File
# Begin Source File

SOURCE=.\d_comp.c
# End Source File
# Begin Source File

SOURCE=.\d_copy.c
# End Source File
# Begin Source File

SOURCE=.\d_deser.c
# End Source File
# Begin Source File

SOURCE=.\d_detach.c
# End Source File
# Begin Source File

SOURCE=.\d_f_each.c
# End Source File
# Begin Source File

SOURCE=.\d_f_that.c
# End Source File
# Begin Source File

SOURCE=.\d_find.c
# End Source File
# Begin Source File

SOURCE=.\d_find_p.c
# End Source File
# Begin Source File

SOURCE=.\d_free.c
# End Source File
# Begin Source File

SOURCE=.\d_get.c
# End Source File
# Begin Source File

SOURCE=.\d_getval.c
# End Source File
# Begin Source File

SOURCE=.\d_init.c
# End Source File
# Begin Source File

SOURCE=.\d_insert.c
# End Source File
# Begin Source File

SOURCE=.\d_merge.c
# End Source File
# Begin Source File

SOURCE=.\d_mult.c
# End Source File
# Begin Source File

SOURCE=.\d_remove.c
# End Source File
# Begin Source File

SOURCE=.\d_rempr.c
# End Source File
# Begin Source File

SOURCE=.\d_repl_i.c
# End Source File
# Begin Source File

SOURCE=.\d_replce.c
# End Source File
# Begin Source File

SOURCE=.\d_replpr.c
# End Source File
# Begin Source File

SOURCE=.\d_ser.c
# End Source File
# Begin Source File

SOURCE=.\d_setval.c
# End Source File
# Begin Source File

SOURCE=.\p_arr.c
# End Source File
# Begin Source File

SOURCE=.\p_catch.c
# End Source File
# Begin Source File

SOURCE=.\p_copy.c
# End Source File
# Begin Source File

SOURCE=.\p_free.c
# End Source File
# Begin Source File

SOURCE=.\p_get.c
# End Source File
# Begin Source File

SOURCE=.\p_init.c
# End Source File
# Begin Source File

SOURCE=.\p_mult.c
# End Source File
# Begin Source File

SOURCE=.\p_predct.c
# End Source File
# Begin Source File

SOURCE=.\p_replce.c
# End Source File
# Begin Source File

SOURCE=.\pa_gitms.c
# End Source File
# Begin Source File

SOURCE=.\pa_insrt.c
# End Source File
# Begin Source File

SOURCE=.\pa_remve.c
# End Source File
# Begin Source File

SOURCE=.\pa_sitms.c
# End Source File
# Begin Source File

SOURCE=.\prop_wrp.cpp
# End Source File
# Begin Source File

SOURCE=.\sec.c
# End Source File
# Begin Source File

SOURCE=.\sec_add.c
# End Source File
# Begin Source File

SOURCE=.\sec_allc.c
# End Source File
# Begin Source File

SOURCE=.\sec_comp.c
# End Source File
# Begin Source File

SOURCE=.\sec_make.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\avp_data.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\AVPPort.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\byteorder.h
# End Source File
# Begin Source File

SOURCE=.\prop_in.h
# End Source File
# Begin Source File

SOURCE=.\Prop_wrp.h
# End Source File
# Begin Source File

SOURCE=.\Property.h
# End Source File
# End Group
# End Target
# End Project

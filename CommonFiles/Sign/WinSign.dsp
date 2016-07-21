# Microsoft Developer Studio Project File - Name="WinSign" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WinSign - Win32 DebugDll With Debug Key
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinSign.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinSign.mak" CFG="WinSign - Win32 DebugDll With Debug Key"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinSign - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 Debug With Debug Key" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 DebugDll With Debug Key" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 Novell_Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Sign", LBDAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinSign - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GR /GX /O2 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/ReleaseS\Sign.lib" /LINK50COMPAT
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x417
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/WinSign.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/DebugS\Sign.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinSign___Win32_DebugDll"
# PROP BASE Intermediate_Dir "WinSign___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /GR /GX /ZI /Od /I "..\LCrypto\Include" /I "..\Common" /I "..\..\CommonFiles" /I "..\..\iosys" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /FR /FD /GZ /c
# ADD BASE RSC /l 0x417
# ADD RSC /l 0x417
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"DebugDll/WinSign.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\DebugS\Sign.lib"
# ADD LIB32 /nologo /out:"../../Out/Debug\Sign.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinSign___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "WinSign___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GR /GX /O2 /I "..\LCrypto\Include" /I "..\Common" /I "..\..\CommonFiles" /I "..\..\iosys" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GR /GX /O2 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\ReleaseS\Sign.lib" /LINK50COMPAT
# ADD LIB32 /nologo /out:"../../Out/Release\Sign.lib" /LINK50COMPAT
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinSign___Win32_Debug_With_Debug_Key"
# PROP BASE Intermediate_Dir "WinSign___Win32_Debug_With_Debug_Key"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugWDKey/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "..\Common" /I "..\..\CommonFiles" /I "..\..\iosys" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /FR /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_DEBUG_KEY" /FR /FD /GZ /c
# ADD BASE RSC /l 0x417
# ADD RSC /l 0x417
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/Sign_wdk.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\DebugS\Sign.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugS\Sign_wdk.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\DebugS\Sign_wdk.lib
InputName=Sign_wdk
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinSign___Win32_DebugDll_With_Debug_Key"
# PROP BASE Intermediate_Dir "WinSign___Win32_DebugDll_With_Debug_Key"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/DebugDllWDKey/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "..\Common" /I "..\..\CommonFiles" /I "..\..\iosys" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /FR /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG_KEY" /FR /FD /GZ /c
# ADD BASE RSC /l 0x417
# ADD RSC /l 0x417
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"DebugDll/Sign_wdk.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\Debug\Sign.lib"
# ADD LIB32 /nologo /out:"../../Out/Debug\Sign_wdk.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\Debug\Sign_wdk.lib
InputName=Sign_wdk
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinSign___Win32_Novell_Release"
# PROP BASE Intermediate_Dir "WinSign___Win32_Novell_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/Novell_Release/CommonFiles/Sign"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GR /GX /O2 /I "..\Common" /I "..\..\CommonFiles" /I "..\..\iosys" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /O2 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /D "__WATCOMC__" /D "N_PLAT_NLM" /FA /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\CommonFiles\ReleaseS\Sign.lib" /LINK50COMPAT
# ADD LIB32 /nologo /out:"../../Out/ReleaseS\Sign_n.lib" /LINK50COMPAT
# Begin Custom Build - Copying $(InputPath) to CommonFiles...
InputPath=\Out\ReleaseS\Sign_n.lib
InputName=Sign_n
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ENDIF 

# Begin Target

# Name "WinSign - Win32 Release"
# Name "WinSign - Win32 Debug"
# Name "WinSign - Win32 DebugDll"
# Name "WinSign - Win32 ReleaseDll"
# Name "WinSign - Win32 Debug With Debug Key"
# Name "WinSign - Win32 DebugDll With Debug Key"
# Name "WinSign - Win32 Novell_Release"
# Begin Group "Objs"

# PROP Default_Filter "*.obj"
# Begin Source File

SOURCE=.\objs\c_coderf.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_crc16f.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_crc32.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_crypto.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_decryp.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_find.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_fs.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_hexcod.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_mem.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_randfi.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_randna.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_srand.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\c_swdict.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\c_tm.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\c_urand.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\c_uucode.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_verify.obj
# End Source File
# Begin Source File

SOURCE=.\objs\c_vesta.obj
# End Source File
# Begin Source File

SOURCE=.\objs\ct_touch.obj
# End Source File
# Begin Source File

SOURCE=.\objs\ct_ver.obj
# End Source File
# Begin Source File

SOURCE=.\objs\f_not.obj
# End Source File
# Begin Source File

SOURCE=.\objs\f_sha.obj
# End Source File
# Begin Source File

SOURCE=.\objs\f_sign.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\n_addkey.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_constr.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_dsa.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_file.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\n_fpub.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_gostrf.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\n_hash.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_privat.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\n_public.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_r50.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_signrf.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_signus.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_touch.obj
# End Source File
# Begin Source File

SOURCE=.\objs\n_vault.obj
# End Source File
# Begin Source File

SOURCE=.\objs\w_shnor.obj
# End Source File
# Begin Source File

SOURCE=.\objs\w_signsh.obj
# End Source File
# Begin Source File

SOURCE=.\objs\cn_urand.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objs\cn_tm.obj

!IF  "$(CFG)" == "WinSign - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Src"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\src\errno.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\my_strl.c
# End Source File
# Begin Source File

SOURCE=.\src\n_data.c
# End Source File
# Begin Source File

SOURCE=.\src\s_buflen.c
# End Source File
# Begin Source File

SOURCE=.\src\s_chbuf.c
# End Source File
# Begin Source File

SOURCE=.\src\s_check.c
# End Source File
# Begin Source File

SOURCE=.\src\s_chfile.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_chfilw.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_chfres.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_chres.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_cnot.c
# End Source File
# Begin Source File

SOURCE=.\src\s_crpriv.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_except.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fnguid.c
# End Source File
# Begin Source File

SOURCE=.\src\s_fnstr.c
# End Source File
# Begin Source File

SOURCE=.\src\s_hash.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

# ADD CPP /O1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

# ADD BASE CPP /O1
# ADD CPP /O1

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# ADD BASE CPP /O1
# ADD CPP /O1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_klok.c
# End Source File
# Begin Source File

SOURCE=.\src\s_lnfile.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_lnot.c
# End Source File
# Begin Source File

SOURCE=.\src\s_putn.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_sgnfil.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_sgnmem.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_sifile.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_sign.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_spkey.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\s_strver.c
# End Source File
# Begin Source File

SOURCE=..\stuff\text2bin.c
# End Source File
# End Group
# Begin Group "Hdr"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\avp_base.h
# End Source File
# Begin Source File

SOURCE=..\AVP_data.h
# End Source File
# Begin Source File

SOURCE=.\avp_defs.h
# End Source File
# Begin Source File

SOURCE=..\AVPPort.h
# End Source File
# Begin Source File

SOURCE=..\byteorder.h
# End Source File
# Begin Source File

SOURCE=.\ct_defs.h
# End Source File
# Begin Source File

SOURCE=.\ct_fio.h
# End Source File
# Begin Source File

SOURCE=.\ct_funcs.h
# End Source File
# Begin Source File

SOURCE=.\ct_strct.h
# End Source File
# Begin Source File

SOURCE=.\ct_sup_d.h
# End Source File
# Begin Source File

SOURCE=.\ct_supp.h
# End Source File
# Begin Source File

SOURCE=.\ct_typs.h
# End Source File
# Begin Source File

SOURCE=.\sign.h
# End Source File
# Begin Source File

SOURCE=.\sign_lib.h
# End Source File
# Begin Source File

SOURCE=.\signport.h
# End Source File
# Begin Source File

SOURCE=..\stuff\text2bin.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\WinSign.dep
# End Source File
# Begin Source File

SOURCE=.\WinSign.mak
# End Source File
# End Target
# End Project

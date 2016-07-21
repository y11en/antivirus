# Microsoft Developer Studio Project File - Name="SWManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SWManager - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SWManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SWManager.mak" CFG="SWManager - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SWManager - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/SWManager""
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SWManager - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/ReleaseS\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Sources\5.Dev\Out\ReleaseS\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/DebugS\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Sources\5.Dev\Out\DebugS\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SWManager___Win32_DebugDll"
# PROP BASE Intermediate_Dir "SWManager___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\DebugS\SWM.lib"
# ADD LIB32 /nologo /out:"../../Out/Debug\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Sources\5.Dev\Out\Debug\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SWManager___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "SWManager___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\ReleaseS\SWM.lib"
# ADD LIB32 /nologo /out:"../../Out/Release\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Sources\5.Dev\Out\Release\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SWManager___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "SWManager___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugU"
# PROP Intermediate_Dir "../../Temp/DebugU/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Debug\SWM.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugU\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugUDll
InputPath=\Sources\5.Dev\Out\DebugU\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SWManager___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "SWManager___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../Temp/ReleaseUS/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\ReleaseS\SWM.lib"
# ADD LIB32 /nologo /out:"../../Out/ReleaseUS\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseU
InputPath=\Sources\5.Dev\Out\ReleaseUS\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SWManager___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "SWManager___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugUS"
# PROP Intermediate_Dir "../../Temp/DebugUS/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\DebugS\SWM.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugUS\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugU
InputPath=\Sources\5.Dev\Out\DebugUS\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugU

# End Custom Build

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SWManager___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "SWManager___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseU"
# PROP Intermediate_Dir "../../Temp/ReleaseU/CommonFiles/SWManager"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Release\SWM.lib"
# ADD LIB32 /nologo /out:"../../Out/ReleaseU\SWM.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseUDll
InputPath=\Sources\5.Dev\Out\ReleaseU\SWM.lib
InputName=SWM
SOURCE="$(InputPath)"

"..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseUDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "SWManager - Win32 Release"
# Name "SWManager - Win32 Debug"
# Name "SWManager - Win32 DebugDll"
# Name "SWManager - Win32 ReleaseDll"
# Name "SWManager - Win32 Unicode DebugDll"
# Name "SWManager - Win32 Unicode Release"
# Name "SWManager - Win32 Unicode Debug"
# Name "SWManager - Win32 Unicode ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Checker.c
# End Source File
# Begin Source File

SOURCE=.\CheckerR.c
# End Source File
# Begin Source File

SOURCE=.\CheckerW.c
# End Source File
# Begin Source File

SOURCE=.\Encrypt.c
# End Source File
# Begin Source File

SOURCE=.\EncryptR.c
# End Source File
# Begin Source File

SOURCE=.\EncryptW.c
# End Source File
# Begin Source File

SOURCE=.\Manager.c
# End Source File
# Begin Source File

SOURCE=.\ManagerR.c
# End Source File
# Begin Source File

SOURCE=.\ManagerW.c
# End Source File
# Begin Source File

SOURCE=.\Packer.c
# End Source File
# Begin Source File

SOURCE=.\PackerR.c
# End Source File
# Begin Source File

SOURCE=.\PackerW.c
# End Source File
# Begin Source File

SOURCE=.\RawData.c
# End Source File
# Begin Source File

SOURCE=.\RawDataR.c
# End Source File
# Begin Source File

SOURCE=.\RawDataW.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Checker.h
# End Source File
# Begin Source File

SOURCE=.\Encrypt.h
# End Source File
# Begin Source File

SOURCE=.\Manager.h
# End Source File
# Begin Source File

SOURCE=.\Packer.h
# End Source File
# Begin Source File

SOURCE=.\RawData.h
# End Source File
# Begin Source File

SOURCE=.\Swm.h
# End Source File
# Begin Source File

SOURCE=.\Swmi.h
# End Source File
# End Group
# End Target
# End Project

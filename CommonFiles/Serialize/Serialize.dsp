# Microsoft Developer Studio Project File - Name="Serialize" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Serialize - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Serialize.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Serialize.mak" CFG="Serialize - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Serialize - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/Serialize""
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "Serialize - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/Serialize"
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
# ADD LIB32 /nologo /out:"../../Out/ReleaseS\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/DebugS\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Serialize___Win32_DebugDll"
# PROP BASE Intermediate_Dir "Serialize___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\DebugS\KLDTSer.lib"
# ADD LIB32 /nologo /out:"../../Out/Debug\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Serialize___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "Serialize___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I ".\\" /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\ReleaseS\KLDTSer.lib"
# ADD LIB32 /nologo /out:"../../Out/Release\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Serialize___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "Serialize___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugU"
# PROP Intermediate_Dir "../../Temp/DebugU/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Debug\KLDTSer.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugU\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugUDll
InputPath=\Out\DebugU\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Serialize___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "Serialize___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../Temp/ReleaseUS/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\ReleaseS\KLDTSer.lib"
# ADD LIB32 /nologo /out:"../../Out/ReleaseUS\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseU
InputPath=\Out\ReleaseUS\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Serialize___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "Serialize___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugUS"
# PROP Intermediate_Dir "../../Temp/DebugUS/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\DebugS\KLDTSer.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugUS\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugU
InputPath=\Out\DebugUS\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugU

# End Custom Build

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Serialize___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "Serialize___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseU"
# PROP Intermediate_Dir "../../Temp/ReleaseU/CommonFiles/Serialize"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Release\KLDTSer.lib"
# ADD LIB32 /nologo /out:"../../Out/ReleaseU\KLDTSer.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseUDll
InputPath=\Out\ReleaseU\KLDTSer.lib
InputName=KLDTSer
SOURCE="$(InputPath)"

"..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseUDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "Serialize - Win32 Release"
# Name "Serialize - Win32 Debug"
# Name "Serialize - Win32 DebugDll"
# Name "Serialize - Win32 ReleaseDll"
# Name "Serialize - Win32 Unicode DebugDll"
# Name "Serialize - Win32 Unicode Release"
# Name "Serialize - Win32 Unicode Debug"
# Name "Serialize - Win32 Unicode ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Deser.c
# End Source File
# Begin Source File

SOURCE=.\Deser_W.c
# End Source File
# Begin Source File

SOURCE=.\DeserH_W.c
# End Source File
# Begin Source File

SOURCE=.\DeserHdr.c
# End Source File
# Begin Source File

SOURCE=.\DeserMem.c
# End Source File
# Begin Source File

SOURCE=.\DeserR.c
# End Source File
# Begin Source File

SOURCE=.\DeserR_W.c
# End Source File
# Begin Source File

SOURCE=.\DeserSWM.c
# End Source File
# Begin Source File

SOURCE=.\DeserSWM_W.c
# End Source File
# Begin Source File

SOURCE=.\DesrMHdr.c
# End Source File
# Begin Source File

SOURCE=.\DesrMR.c
# End Source File
# Begin Source File

SOURCE=.\DesrMSWM.c
# End Source File
# Begin Source File

SOURCE=.\SeriaCst.c
# End Source File
# Begin Source File

SOURCE=.\SeriaCst_W.c
# End Source File
# Begin Source File

SOURCE=.\SeriaHdr.c
# End Source File
# Begin Source File

SOURCE=.\SeriaInt.c
# End Source File
# Begin Source File

SOURCE=.\Serial.c
# End Source File
# Begin Source File

SOURCE=.\Serial_W.c
# End Source File
# Begin Source File

SOURCE=.\SerialW.c
# End Source File
# Begin Source File

SOURCE=.\SerialW_W.c
# End Source File
# Begin Source File

SOURCE=.\SeriaMem.c
# End Source File
# Begin Source File

SOURCE=.\SeriaSWM.c
# End Source File
# Begin Source File

SOURCE=.\SeriaSWM_W.c
# End Source File
# Begin Source File

SOURCE=.\SeriMCst.c
# End Source File
# Begin Source File

SOURCE=.\SeriMHdr.c
# End Source File
# Begin Source File

SOURCE=.\SeriMSWM.c
# End Source File
# Begin Source File

SOURCE=.\SeriMW.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\CommonFiles\_avpio.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\avp_data.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\AVPComID.h
# End Source File
# Begin Source File

SOURCE=.\KLDTSer.h
# End Source File
# Begin Source File

SOURCE=..\Property\Property.h
# End Source File
# Begin Source File

SOURCE=..\SWManager\Swm.h
# End Source File
# End Group
# End Target
# End Project

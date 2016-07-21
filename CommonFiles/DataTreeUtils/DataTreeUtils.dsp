# Microsoft Developer Studio Project File - Name="DataTreeUtils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DataTreeUtils - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DataTreeUtils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DataTreeUtils.mak" CFG="DataTreeUtils - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DataTreeUtils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/DataTreeUtils""
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "DataTreeUtils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseS"
# PROP Intermediate_Dir "../../Temp/ReleaseS/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G4 /MT /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/ReleaseS\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\Out\ReleaseS\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugS"
# PROP Intermediate_Dir "../../Temp/DebugS/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Out/DebugS\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\Out\DebugS\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/Debug"
# PROP Intermediate_Dir "../../Temp/Debug/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\DebugS\DTUtils.lib"
# ADD LIB32 /nologo /out:"../../Out/Debug\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\Out\Debug\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DataTreeUtils___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "DataTreeUtils___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/Release"
# PROP Intermediate_Dir "../../Temp/Release/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "D:\AVPDevelopment\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"D:\AVPDevelopment\CommonFiles\ReleaseS\DTUtils.lib"
# ADD LIB32 /nologo /out:"../../Out/Release\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\Out\Release\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DataTreeUtils___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "DataTreeUtils___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugU"
# PROP Intermediate_Dir "../../Temp/DebugU/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Debug\DTUtils.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugU\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugUDll
InputPath=\Out\DebugU\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DataTreeUtils___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "DataTreeUtils___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseU"
# PROP Intermediate_Dir "../../Temp/ReleaseU/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G4 /MD /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\Release\DTUtils.lib"
# ADD LIB32 /nologo /out:"../../Out/ReleaseU\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseUDll
InputPath=\Out\ReleaseU\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DataTreeUtils___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "DataTreeUtils___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Out/ReleaseUS"
# PROP Intermediate_Dir "../../Temp/ReleaseUS/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G4 /MT /W3 /GX /O2 /I "..\CommonFiles" /I "..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\ReleaseS\DTUtils.lib"
# ADD LIB32 /nologo /out:"../../Out/ReleaseUS\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseU
InputPath=\Out\ReleaseUS\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DataTreeUtils___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "DataTreeUtils___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Out/DebugUS"
# PROP Intermediate_Dir "../../Temp/DebugUS/CommonFiles/DataTreeUtils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G4 /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\CommonFiles\DebugS\DTUtils.lib"
# ADD LIB32 /nologo /out:"../../Out/DebugUS\DTUtils.lib"
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugU
InputPath=\Out\DebugUS\DTUtils.lib
InputName=DTUtils
SOURCE="$(InputPath)"

"..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\DebugU

# End Custom Build

!ENDIF 

# Begin Target

# Name "DataTreeUtils - Win32 Release"
# Name "DataTreeUtils - Win32 Debug"
# Name "DataTreeUtils - Win32 DebugDll"
# Name "DataTreeUtils - Win32 ReleaseDll"
# Name "DataTreeUtils - Win32 Unicode DebugDll"
# Name "DataTreeUtils - Win32 Unicode ReleaseDll"
# Name "DataTreeUtils - Win32 Unicode Release"
# Name "DataTreeUtils - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CmpObjs.c
# End Source File
# Begin Source File

SOURCE=.\CmpObjsW.c
# End Source File
# Begin Source File

SOURCE=.\CompTree.c
# End Source File
# Begin Source File

SOURCE=.\DtPtrnME.c
# End Source File
# Begin Source File

SOURCE=.\DTUInit.c
# End Source File
# Begin Source File

SOURCE=.\ExchProp.c
# End Source File
# Begin Source File

SOURCE=.\FindGet.c
# End Source File
# Begin Source File

SOURCE=.\PIDFind.c
# End Source File
# Begin Source File

SOURCE=.\PrSetGet.c
# End Source File
# Begin Source File

SOURCE=.\PrSetGetW.c
# End Source File
# Begin Source File

SOURCE=.\RuleConv.c
# End Source File
# Begin Source File

SOURCE=.\SearRule.c
# End Source File
# Begin Source File

SOURCE=.\SearRuleW.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DTUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\CommonFiles\Stuff\Enc2Text.h
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="Win32Utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Win32Utils - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Win32Utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Win32Utils.mak" CFG="Win32Utils - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32Utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Win32Utils - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/CommonFiles/windows/Win32Utils""
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseS"
# PROP BASE Intermediate_Dir "ReleaseS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\ReleaseS"
# PROP Intermediate_Dir "..\..\..\Temp\ReleaseS\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Release
InputPath=\disk_d\AVPDevelopment\Out\ReleaseS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\Release\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\Release

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugS"
# PROP BASE Intermediate_Dir "DebugS"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\DebugS"
# PROP Intermediate_Dir "..\..\..\Temp\DebugS\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/Debug
InputPath=\disk_d\AVPDevelopment\Out\DebugS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\Debug\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\Debug

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32Utils___Win32_DebugDll"
# PROP BASE Intermediate_Dir "Win32Utils___Win32_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\Debug"
# PROP Intermediate_Dir "..\..\..\Temp\Debug\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\Out\DebugS\Win32Utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugDll
InputPath=\disk_d\AVPDevelopment\Out\Debug\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\DebugDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32Utils___Win32_ReleaseDll"
# PROP BASE Intermediate_Dir "Win32Utils___Win32_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\Release"
# PROP Intermediate_Dir "..\..\..\Temp\Release\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\Out\CommonFiles\ReleaseS\Win32Utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseDll
InputPath=\disk_d\AVPDevelopment\Out\Release\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\ReleaseDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32Utils___Win32_Unicode_DebugDll"
# PROP BASE Intermediate_Dir "Win32Utils___Win32_Unicode_DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\DebugU"
# PROP Intermediate_Dir "..\..\..\Temp\DebugU\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\Out\Debug\Win32Utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugUDll
InputPath=\disk_d\AVPDevelopment\Out\DebugU\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\DebugUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugUDll

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32Utils___Win32_Unicode_Release"
# PROP BASE Intermediate_Dir "Win32Utils___Win32_Unicode_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\ReleaseUS"
# PROP Intermediate_Dir "..\..\..\Temp\ReleaseUS\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\Out\ReleaseS\Win32Utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseU
InputPath=\disk_d\AVPDevelopment\Out\ReleaseUS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\ReleaseU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseU

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win32Utils___Win32_Unicode_Debug"
# PROP BASE Intermediate_Dir "Win32Utils___Win32_Unicode_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Out\DebugUS"
# PROP Intermediate_Dir "..\..\..\Temp\DebugUS\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\Out\DebugS\Win32Utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/DebugU
InputPath=\disk_d\AVPDevelopment\Out\DebugUS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\DebugU\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\DebugU

# End Custom Build

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win32Utils___Win32_Unicode_ReleaseDll"
# PROP BASE Intermediate_Dir "Win32Utils___Win32_Unicode_ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Out\ReleaseU"
# PROP Intermediate_Dir "..\..\..\Temp\ReleaseU\CommonFiles\windows\Win32Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\Out\Release\Win32Utils.lib"
# ADD LIB32 /nologo
# Begin Custom Build - Copying $(InputPath) to CommonFiles/ReleaseUDll
InputPath=\disk_d\AVPDevelopment\Out\ReleaseU\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\ReleaseUDll\$(InputName).lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputPath)" ..\..\ReleaseUDll

# End Custom Build

!ENDIF 

# Begin Target

# Name "Win32Utils - Win32 Release"
# Name "Win32Utils - Win32 Debug"
# Name "Win32Utils - Win32 DebugDll"
# Name "Win32Utils - Win32 ReleaseDll"
# Name "Win32Utils - Win32 Unicode DebugDll"
# Name "Win32Utils - Win32 Unicode Release"
# Name "Win32Utils - Win32 Unicode Debug"
# Name "Win32Utils - Win32 Unicode ReleaseDll"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\Stuff\_carray.cpp

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Stuff\AutoReg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CalcSum.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CLParse.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CmdLine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\CompressPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\COMUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\csystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CType2ID.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\DesrFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\DIBConv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Version\DllCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Enc2Text.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\FAutomat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\FFind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\FFindW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_BasedPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CheckCreatableFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CheckWritableDir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CheckWritableDirW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CheckWritableFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CopyFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CopyFileW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CreateDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_CreateDirectoryW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_DeleteDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_DeleteFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_DeleteFileW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_DirExtract.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_DirExtractW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FileAttributes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FileAttributesW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FileExtract.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FileExtractW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FindFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FindFileExtention.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FindFileExtentionW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FinishCopying.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_FinishCopyingW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetCurrentDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetCurrentDirectoryW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetDriveType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetDriveTypeW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetFileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetFileInfoW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetIsDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetIsDirectoryW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetLogicalDriveStrings.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetLogicalDriveStringsW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetLongPathName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetShortPathName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetShortPathNameW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetTempPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetTempPathW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetVolumeInformation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_GetVolumeInformationW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_IsSubstDrive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_IsSubstDriveW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_MakeFullPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_MakeFullPathW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_MoveFileEx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_MoveFileEx9x.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_MoveFileExW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_QueryDosDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_QueryDosDeviceW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_RelativePath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_RelativePathW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_RenameFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_SetCurrentDirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil_SetCurrentDirectoryW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\IoutilCOMDLG.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\IoutilUNC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\IoutilUNCW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\IoutilW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Layout.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\LoadCompDll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Match.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\MatchW.c
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\MemManag.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\nw_wait.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\OCXReg.cpp

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Stuff\ProcessEnumerator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\RegStorage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\SnakePainter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Text2bin.c
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TextRotator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TODialog.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TOMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TraceLog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\UNCConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Version\versinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\XErrors.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Xfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\XfileW.cpp
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\XFlags.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\Stuff\_carray.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\AutoReg.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CalcSum.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CLParse.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CmdLine.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\combool.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\COMUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\cpointer.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\csystem.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\CType2ID.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\DIBConv.h
# End Source File
# Begin Source File

SOURCE=..\..\Version\DllCheck.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Enc2Text.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\FAutomat.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\FFind.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\FFindW.h
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Ioutil.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Layout.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\LoadCompDll.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\MemManag.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\nw_wait.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\OCXReg.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\OCXStd.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Parray.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Pathstr.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\PathstrA.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\PathstrW.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\ProcessEnumerator.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\PSynArr.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\PSynQue.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\RegStorage.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Sarray.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\securlib.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Slist.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\SnakePainter.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\sock_lib.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\StdDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Text2bin.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TextRotator.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TODialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\TOMsgBox.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Tracelog.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\UNCConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Utils.h
# End Source File
# Begin Source File

SOURCE=..\..\Version\versinfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Stuff\Win95adg.h
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Xerrors.h
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\Xfile.h
# End Source File
# Begin Source File

SOURCE=..\..\StuffIO\XFlags.h
# End Source File
# End Group
# End Target
# End Project

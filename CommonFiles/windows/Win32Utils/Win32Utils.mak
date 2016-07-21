# Microsoft Developer Studio Generated NMAKE File, Based on Win32Utils.dsp
!IF "$(CFG)" == ""
CFG=Win32Utils - Win32 Unicode Debug
!MESSAGE No configuration specified. Defaulting to Win32Utils - Win32 Unicode Debug.
!ENDIF 

!IF "$(CFG)" != "Win32Utils - Win32 Release" && "$(CFG)" != "Win32Utils - Win32 Debug" && "$(CFG)" != "Win32Utils - Win32 DebugDll" && "$(CFG)" != "Win32Utils - Win32 ReleaseDll" && "$(CFG)" != "Win32Utils - Win32 Unicode DebugDll" && "$(CFG)" != "Win32Utils - Win32 Unicode Release" && "$(CFG)" != "Win32Utils - Win32 Unicode Debug" && "$(CFG)" != "Win32Utils - Win32 Unicode ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

OUTDIR=.\..\..\..\Out\ReleaseS
INTDIR=.\..\..\..\Temp\ReleaseS\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\Release\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\Release\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\ReleaseS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\Release\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\Release
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

OUTDIR=.\..\..\..\Out\DebugS
INTDIR=.\..\..\..\Temp\DebugS\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\DebugS
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\Debug\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\Debug\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\DebugS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\Debug\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

OUTDIR=.\..\..\..\Out\Debug
INTDIR=.\..\..\..\Temp\Debug\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\DebugDll\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\DebugDll\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\Debug\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\DebugDll\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

OUTDIR=.\..\..\..\Out\Release
INTDIR=.\..\..\..\Temp\Release\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\Release
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\ReleaseDll\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\ReleaseDll\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\Release\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\ReleaseDll\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseDll
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

OUTDIR=.\..\..\..\Out\DebugU
INTDIR=.\..\..\..\Temp\DebugU\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\DebugU
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\DebugUDll\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\DebugUDll\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\DebugU\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\DebugUDll\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugUDll
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

OUTDIR=.\..\..\..\Out\ReleaseUS
INTDIR=.\..\..\..\Temp\ReleaseUS\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\ReleaseUS
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\ReleaseU\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\ReleaseU\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\ReleaseUS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\ReleaseU\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseU
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

OUTDIR=.\..\..\..\Out\DebugUS
INTDIR=.\..\..\..\Temp\DebugUS\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\DebugUS
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\DebugU\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\DebugU\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\DebugUS\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\DebugU\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugU
<< 
	

!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

OUTDIR=.\..\..\..\Out\ReleaseU
INTDIR=.\..\..\..\Temp\ReleaseU\CommonFiles\windows\Win32Utils
# Begin Custom Macros
OutDir=.\..\..\..\Out\ReleaseU
# End Custom Macros

ALL : "$(OUTDIR)\Win32Utils.lib" "..\..\ReleaseUDll\Win32Utils.lib"


CLEAN :
	-@erase "$(INTDIR)\AutoReg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CLParse.obj"
	-@erase "$(INTDIR)\CmdLine.obj"
	-@erase "$(INTDIR)\CompressPath.obj"
	-@erase "$(INTDIR)\COMUtils.obj"
	-@erase "$(INTDIR)\csystem.obj"
	-@erase "$(INTDIR)\CType2ID.obj"
	-@erase "$(INTDIR)\DesrFile.obj"
	-@erase "$(INTDIR)\DIBConv.obj"
	-@erase "$(INTDIR)\DllCheck.obj"
	-@erase "$(INTDIR)\Enc2Text.obj"
	-@erase "$(INTDIR)\FAutomat.obj"
	-@erase "$(INTDIR)\FFind.obj"
	-@erase "$(INTDIR)\FFindW.obj"
	-@erase "$(INTDIR)\Ioutil.obj"
	-@erase "$(INTDIR)\Ioutil_BasedPath.obj"
	-@erase "$(INTDIR)\Ioutil_CheckCreatableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDir.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableDirW.obj"
	-@erase "$(INTDIR)\Ioutil_CheckWritableFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFile.obj"
	-@erase "$(INTDIR)\Ioutil_CopyFileW.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_CreateDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFile.obj"
	-@erase "$(INTDIR)\Ioutil_DeleteFileW.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtract.obj"
	-@erase "$(INTDIR)\Ioutil_DirExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributes.obj"
	-@erase "$(INTDIR)\Ioutil_FileAttributesW.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtract.obj"
	-@erase "$(INTDIR)\Ioutil_FileExtractW.obj"
	-@erase "$(INTDIR)\Ioutil_FindFile.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtention.obj"
	-@erase "$(INTDIR)\Ioutil_FindFileExtentionW.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopying.obj"
	-@erase "$(INTDIR)\Ioutil_FinishCopyingW.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveType.obj"
	-@erase "$(INTDIR)\Ioutil_GetDriveTypeW.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfo.obj"
	-@erase "$(INTDIR)\Ioutil_GetFileInfoW.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_GetIsDirectoryW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj"
	-@erase "$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"
	-@erase "$(INTDIR)\Ioutil_GetLongPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathName.obj"
	-@erase "$(INTDIR)\Ioutil_GetShortPathNameW.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPath.obj"
	-@erase "$(INTDIR)\Ioutil_GetTempPathW.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformation.obj"
	-@erase "$(INTDIR)\Ioutil_GetVolumeInformationW.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDrive.obj"
	-@erase "$(INTDIR)\Ioutil_IsSubstDriveW.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPath.obj"
	-@erase "$(INTDIR)\Ioutil_MakeFullPathW.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileEx9x.obj"
	-@erase "$(INTDIR)\Ioutil_MoveFileExW.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDevice.obj"
	-@erase "$(INTDIR)\Ioutil_QueryDosDeviceW.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePath.obj"
	-@erase "$(INTDIR)\Ioutil_RelativePathW.obj"
	-@erase "$(INTDIR)\Ioutil_RenameFile.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectory.obj"
	-@erase "$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj"
	-@erase "$(INTDIR)\IoutilCOMDLG.obj"
	-@erase "$(INTDIR)\IoutilUNC.obj"
	-@erase "$(INTDIR)\IoutilUNCW.obj"
	-@erase "$(INTDIR)\IoutilW.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCompDll.obj"
	-@erase "$(INTDIR)\Match.obj"
	-@erase "$(INTDIR)\MatchW.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\nw_wait.obj"
	-@erase "$(INTDIR)\OCXReg.obj"
	-@erase "$(INTDIR)\ProcessEnumerator.obj"
	-@erase "$(INTDIR)\RegStorage.obj"
	-@erase "$(INTDIR)\SnakePainter.obj"
	-@erase "$(INTDIR)\Text2bin.obj"
	-@erase "$(INTDIR)\TextRotator.obj"
	-@erase "$(INTDIR)\thread.obj"
	-@erase "$(INTDIR)\TODialog.obj"
	-@erase "$(INTDIR)\TOMsgBox.obj"
	-@erase "$(INTDIR)\TraceLog.obj"
	-@erase "$(INTDIR)\UNCConvert.obj"
	-@erase "$(INTDIR)\Utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\versinfo.obj"
	-@erase "$(INTDIR)\XErrors.obj"
	-@erase "$(INTDIR)\Xfile.obj"
	-@erase "$(INTDIR)\XfileW.obj"
	-@erase "$(INTDIR)\XFlags.obj"
	-@erase "$(OUTDIR)\Win32Utils.lib"
	-@erase "..\..\ReleaseUDll\Win32Utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Win32Utils.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Win32Utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AutoReg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CLParse.obj" \
	"$(INTDIR)\CmdLine.obj" \
	"$(INTDIR)\CompressPath.obj" \
	"$(INTDIR)\COMUtils.obj" \
	"$(INTDIR)\csystem.obj" \
	"$(INTDIR)\CType2ID.obj" \
	"$(INTDIR)\DesrFile.obj" \
	"$(INTDIR)\DIBConv.obj" \
	"$(INTDIR)\DllCheck.obj" \
	"$(INTDIR)\Enc2Text.obj" \
	"$(INTDIR)\FAutomat.obj" \
	"$(INTDIR)\FFind.obj" \
	"$(INTDIR)\FFindW.obj" \
	"$(INTDIR)\Ioutil.obj" \
	"$(INTDIR)\Ioutil_BasedPath.obj" \
	"$(INTDIR)\Ioutil_CheckCreatableFile.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDir.obj" \
	"$(INTDIR)\Ioutil_CheckWritableDirW.obj" \
	"$(INTDIR)\Ioutil_CheckWritableFile.obj" \
	"$(INTDIR)\Ioutil_CopyFile.obj" \
	"$(INTDIR)\Ioutil_CopyFileW.obj" \
	"$(INTDIR)\Ioutil_CreateDirectory.obj" \
	"$(INTDIR)\Ioutil_CreateDirectoryW.obj" \
	"$(INTDIR)\Ioutil_DeleteDirectory.obj" \
	"$(INTDIR)\Ioutil_DeleteFile.obj" \
	"$(INTDIR)\Ioutil_DeleteFileW.obj" \
	"$(INTDIR)\Ioutil_DirExtract.obj" \
	"$(INTDIR)\Ioutil_DirExtractW.obj" \
	"$(INTDIR)\Ioutil_FileAttributes.obj" \
	"$(INTDIR)\Ioutil_FileAttributesW.obj" \
	"$(INTDIR)\Ioutil_FileExtract.obj" \
	"$(INTDIR)\Ioutil_FileExtractW.obj" \
	"$(INTDIR)\Ioutil_FindFile.obj" \
	"$(INTDIR)\Ioutil_FindFileExtention.obj" \
	"$(INTDIR)\Ioutil_FindFileExtentionW.obj" \
	"$(INTDIR)\Ioutil_FinishCopying.obj" \
	"$(INTDIR)\Ioutil_FinishCopyingW.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetDriveType.obj" \
	"$(INTDIR)\Ioutil_GetDriveTypeW.obj" \
	"$(INTDIR)\Ioutil_GetFileInfo.obj" \
	"$(INTDIR)\Ioutil_GetFileInfoW.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectory.obj" \
	"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" \
	"$(INTDIR)\Ioutil_GetLongPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathName.obj" \
	"$(INTDIR)\Ioutil_GetShortPathNameW.obj" \
	"$(INTDIR)\Ioutil_GetTempPath.obj" \
	"$(INTDIR)\Ioutil_GetTempPathW.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformation.obj" \
	"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" \
	"$(INTDIR)\Ioutil_IsSubstDrive.obj" \
	"$(INTDIR)\Ioutil_IsSubstDriveW.obj" \
	"$(INTDIR)\Ioutil_MakeFullPath.obj" \
	"$(INTDIR)\Ioutil_MakeFullPathW.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx.obj" \
	"$(INTDIR)\Ioutil_MoveFileEx9x.obj" \
	"$(INTDIR)\Ioutil_MoveFileExW.obj" \
	"$(INTDIR)\Ioutil_RelativePath.obj" \
	"$(INTDIR)\Ioutil_RelativePathW.obj" \
	"$(INTDIR)\Ioutil_RenameFile.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" \
	"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" \
	"$(INTDIR)\IoutilCOMDLG.obj" \
	"$(INTDIR)\IoutilUNC.obj" \
	"$(INTDIR)\IoutilW.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCompDll.obj" \
	"$(INTDIR)\Match.obj" \
	"$(INTDIR)\MatchW.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\nw_wait.obj" \
	"$(INTDIR)\OCXReg.obj" \
	"$(INTDIR)\ProcessEnumerator.obj" \
	"$(INTDIR)\RegStorage.obj" \
	"$(INTDIR)\SnakePainter.obj" \
	"$(INTDIR)\Text2bin.obj" \
	"$(INTDIR)\TextRotator.obj" \
	"$(INTDIR)\thread.obj" \
	"$(INTDIR)\TODialog.obj" \
	"$(INTDIR)\TOMsgBox.obj" \
	"$(INTDIR)\TraceLog.obj" \
	"$(INTDIR)\UNCConvert.obj" \
	"$(INTDIR)\Utils.obj" \
	"$(INTDIR)\versinfo.obj" \
	"$(INTDIR)\XErrors.obj" \
	"$(INTDIR)\Xfile.obj" \
	"$(INTDIR)\XfileW.obj" \
	"$(INTDIR)\XFlags.obj" \
	"$(INTDIR)\IoutilUNCW.obj" \
	"$(INTDIR)\Ioutil_QueryDosDevice.obj" \
	"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" \
	"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj"

"$(OUTDIR)\Win32Utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\disk_d\AVPDevelopment\Out\ReleaseU\Win32Utils.lib
InputName=Win32Utils
SOURCE="$(InputPath)"

"..\..\ReleaseUDll\Win32Utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseUDll
<< 
	

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Win32Utils.dep")
!INCLUDE "Win32Utils.dep"
!ELSE 
!MESSAGE Warning: cannot find "Win32Utils.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Win32Utils - Win32 Release" || "$(CFG)" == "Win32Utils - Win32 Debug" || "$(CFG)" == "Win32Utils - Win32 DebugDll" || "$(CFG)" == "Win32Utils - Win32 ReleaseDll" || "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll" || "$(CFG)" == "Win32Utils - Win32 Unicode Release" || "$(CFG)" == "Win32Utils - Win32 Unicode Debug" || "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"
SOURCE=..\..\Stuff\_carray.cpp
SOURCE=..\..\Stuff\AutoReg.cpp

"$(INTDIR)\AutoReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\CalcSum.c

"$(INTDIR)\CalcSum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\CLParse.cpp

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CLParse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\Stuff\CmdLine.cpp

"$(INTDIR)\CmdLine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\CompressPath.cpp

"$(INTDIR)\CompressPath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\COMUtils.cpp

"$(INTDIR)\COMUtils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\csystem.cpp

"$(INTDIR)\csystem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\CType2ID.cpp

"$(INTDIR)\CType2ID.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\DesrFile.cpp

"$(INTDIR)\DesrFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\DIBConv.cpp

"$(INTDIR)\DIBConv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Version\DllCheck.cpp

"$(INTDIR)\DllCheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\Enc2Text.c

"$(INTDIR)\Enc2Text.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\FAutomat.cpp

"$(INTDIR)\FAutomat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\FFind.cpp

"$(INTDIR)\FFind.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\FFindW.cpp

"$(INTDIR)\FFindW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil.cpp

"$(INTDIR)\Ioutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_BasedPath.cpp

"$(INTDIR)\Ioutil_BasedPath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CheckCreatableFile.cpp

"$(INTDIR)\Ioutil_CheckCreatableFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CheckWritableDir.cpp

"$(INTDIR)\Ioutil_CheckWritableDir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CheckWritableDirW.cpp

"$(INTDIR)\Ioutil_CheckWritableDirW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CheckWritableFile.cpp

"$(INTDIR)\Ioutil_CheckWritableFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CopyFile.cpp

"$(INTDIR)\Ioutil_CopyFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CopyFileW.cpp

"$(INTDIR)\Ioutil_CopyFileW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CreateDirectory.cpp

"$(INTDIR)\Ioutil_CreateDirectory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_CreateDirectoryW.cpp

"$(INTDIR)\Ioutil_CreateDirectoryW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_DeleteDirectory.cpp

"$(INTDIR)\Ioutil_DeleteDirectory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_DeleteFile.cpp

"$(INTDIR)\Ioutil_DeleteFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_DeleteFileW.cpp

"$(INTDIR)\Ioutil_DeleteFileW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_DirExtract.cpp

"$(INTDIR)\Ioutil_DirExtract.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_DirExtractW.cpp

"$(INTDIR)\Ioutil_DirExtractW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FileAttributes.cpp

"$(INTDIR)\Ioutil_FileAttributes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FileAttributesW.cpp

"$(INTDIR)\Ioutil_FileAttributesW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FileExtract.cpp

"$(INTDIR)\Ioutil_FileExtract.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FileExtractW.cpp

"$(INTDIR)\Ioutil_FileExtractW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FindFile.cpp

"$(INTDIR)\Ioutil_FindFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FindFileExtention.cpp

"$(INTDIR)\Ioutil_FindFileExtention.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FindFileExtentionW.cpp

"$(INTDIR)\Ioutil_FindFileExtentionW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FinishCopying.cpp

"$(INTDIR)\Ioutil_FinishCopying.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_FinishCopyingW.cpp

"$(INTDIR)\Ioutil_FinishCopyingW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetCurrentDirectory.cpp

"$(INTDIR)\Ioutil_GetCurrentDirectory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetCurrentDirectoryW.cpp

"$(INTDIR)\Ioutil_GetCurrentDirectoryW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetDriveType.cpp

"$(INTDIR)\Ioutil_GetDriveType.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetDriveTypeW.cpp

"$(INTDIR)\Ioutil_GetDriveTypeW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetFileInfo.cpp

"$(INTDIR)\Ioutil_GetFileInfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetFileInfoW.cpp

"$(INTDIR)\Ioutil_GetFileInfoW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetIsDirectory.cpp

"$(INTDIR)\Ioutil_GetIsDirectory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetIsDirectoryW.cpp

"$(INTDIR)\Ioutil_GetIsDirectoryW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetLogicalDriveStrings.cpp

"$(INTDIR)\Ioutil_GetLogicalDriveStrings.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetLogicalDriveStringsW.cpp

"$(INTDIR)\Ioutil_GetLogicalDriveStringsW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetLongPathName.cpp

"$(INTDIR)\Ioutil_GetLongPathName.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetShortPathName.cpp

"$(INTDIR)\Ioutil_GetShortPathName.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetShortPathNameW.cpp

"$(INTDIR)\Ioutil_GetShortPathNameW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetTempPath.cpp

"$(INTDIR)\Ioutil_GetTempPath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetTempPathW.cpp

"$(INTDIR)\Ioutil_GetTempPathW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetVolumeInformation.cpp

"$(INTDIR)\Ioutil_GetVolumeInformation.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_GetVolumeInformationW.cpp

"$(INTDIR)\Ioutil_GetVolumeInformationW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_IsSubstDrive.cpp

"$(INTDIR)\Ioutil_IsSubstDrive.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_IsSubstDriveW.cpp

"$(INTDIR)\Ioutil_IsSubstDriveW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_MakeFullPath.cpp

"$(INTDIR)\Ioutil_MakeFullPath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_MakeFullPathW.cpp

"$(INTDIR)\Ioutil_MakeFullPathW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_MoveFileEx.cpp

"$(INTDIR)\Ioutil_MoveFileEx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_MoveFileEx9x.cpp

"$(INTDIR)\Ioutil_MoveFileEx9x.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_MoveFileExW.cpp

"$(INTDIR)\Ioutil_MoveFileExW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_QueryDosDevice.cpp

"$(INTDIR)\Ioutil_QueryDosDevice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_QueryDosDeviceW.cpp

"$(INTDIR)\Ioutil_QueryDosDeviceW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_RelativePath.cpp

"$(INTDIR)\Ioutil_RelativePath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_RelativePathW.cpp

"$(INTDIR)\Ioutil_RelativePathW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_RenameFile.cpp

"$(INTDIR)\Ioutil_RenameFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_SetCurrentDirectory.cpp

"$(INTDIR)\Ioutil_SetCurrentDirectory.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Ioutil_SetCurrentDirectoryW.cpp

"$(INTDIR)\Ioutil_SetCurrentDirectoryW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\IoutilCOMDLG.cpp

"$(INTDIR)\IoutilCOMDLG.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\IoutilUNC.cpp

"$(INTDIR)\IoutilUNC.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\IoutilUNCW.cpp

"$(INTDIR)\IoutilUNCW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\IoutilW.cpp

"$(INTDIR)\IoutilW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\Layout.c

"$(INTDIR)\Layout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\LoadCompDll.cpp

"$(INTDIR)\LoadCompDll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\Match.c

"$(INTDIR)\Match.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\MatchW.c

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\MatchW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\Stuff\MemManag.c

"$(INTDIR)\MemManag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\nw_wait.c

"$(INTDIR)\nw_wait.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\OCXReg.cpp

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\OCXReg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\Stuff\ProcessEnumerator.cpp

"$(INTDIR)\ProcessEnumerator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\RegStorage.cpp

"$(INTDIR)\RegStorage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\SnakePainter.cpp

"$(INTDIR)\SnakePainter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\Text2bin.c

"$(INTDIR)\Text2bin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\TextRotator.cpp

"$(INTDIR)\TextRotator.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\thread.cpp

"$(INTDIR)\thread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\TODialog.cpp

!IF  "$(CFG)" == "Win32Utils - Win32 Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode DebugDll"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\.." /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Win32Utils - Win32 Unicode ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GX /O2 /I "..\.." /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\TODialog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\Stuff\TOMsgBox.cpp

"$(INTDIR)\TOMsgBox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\TraceLog.cpp

"$(INTDIR)\TraceLog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Stuff\UNCConvert.cpp

"$(INTDIR)\UNCConvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Utils.cpp

"$(INTDIR)\Utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Version\versinfo.cpp

"$(INTDIR)\versinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\XErrors.cpp

"$(INTDIR)\XErrors.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\Xfile.cpp

"$(INTDIR)\Xfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\XfileW.cpp

"$(INTDIR)\XfileW.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\StuffIO\XFlags.cpp

"$(INTDIR)\XFlags.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 


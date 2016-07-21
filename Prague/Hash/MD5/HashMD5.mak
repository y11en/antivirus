# Microsoft Developer Studio Generated NMAKE File, Based on HashMD5.dsp
!IF "$(CFG)" == ""
CFG=HashMD5 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to HashMD5 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HashMD5 - Win32 ReleaseWithTrace" && "$(CFG)" != "HashMD5 - Win32 Release" && "$(CFG)" != "HashMD5 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HashMD5.mak" CFG="HashMD5 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HashMD5 - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HashMD5 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HashMD5 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HashMD5 - Win32 ReleaseWithTrace"

OUTDIR=.\../../../out/ReleaseWithTrace
INTDIR=.\../../../temp/ReleaseWithTrace/prague/Hash/MD5
# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

ALL : "$(OUTDIR)\HASHMD5.PPL" ".\../../../temp/ReleaseWithTrace/prague/Hash/MD5\custombuilddone.flg"


CLEAN :
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_hash_md5.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\HASHMD5.EXP"
	-@erase "$(OUTDIR)\HASHMD5.PPL"
	-@erase ".\../../../temp/ReleaseWithTrace/prague/Hash/MD5\custombuilddone.flg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fp"$(INTDIR)\HashMD5.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HashMD5.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\HASHMD5.pdb" /machine:I386 /nodefaultlib /out:"$(OUTDIR)\HASHMD5.PPL" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_hash_md5.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\HASHMD5.PPL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

IntDir=.\../../../temp/ReleaseWithTrace/prague/Hash/MD5
TargetPath=\out\ReleaseWithTrace\HASHMD5.PPL
InputPath=\out\ReleaseWithTrace\HASHMD5.PPL
SOURCE="$(InputPath)"

"..\..\..\temp\ReleaseWithTrace\prague\Hash\MD5\custombuilddone.flg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if exist "$(TargetPath)" echo Converting "$(TargetPath)" 
	if exist "$(TargetPath)" call prconverter /c /b "$(TargetPath)" 
	if exist "$(TargetPath)" echo Signing "$(TargetPath)"... 
	if exist "$(TargetPath)" call tsigner "$(TargetPath)" 
	echo It is safe to delete this file.>"$(IntDir)\custombuilddone.flg" 
<< 
	
TargetPath=\out\ReleaseWithTrace\HASHMD5.PPL
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\HASHMD5.PPL" ".\../../../temp/ReleaseWithTrace/prague/Hash/MD5\custombuilddone.flg"
   call prconvert "\out\ReleaseWithTrace\HASHMD5.PPL"
	tsigner "\out\ReleaseWithTrace\HASHMD5.PPL"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "HashMD5 - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Hash/MD5
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\HASHMD5.PPL"


CLEAN :
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_hash_md5.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HASHMD5.exp"
	-@erase "$(OUTDIR)\HASHMD5.pdb"
	-@erase "$(OUTDIR)\HASHMD5.PPL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HASHMD5_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HashMD5.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x63f00000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\HASHMD5.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\HASHMD5.PPL" /implib:"$(OUTDIR)\HASHMD5.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_hash_md5.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\HASHMD5.PPL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\HASHMD5.PPL
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\HASHMD5.PPL"
   call prconvert "\out\Release\HASHMD5.PPL"
	tsigner "\out\Release\HASHMD5.PPL"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "HashMD5 - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Hash/MD5
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\HASHMD5.PPL"


CLEAN :
	-@erase "$(INTDIR)\hash.obj"
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_hash_md5.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HASHMD5.EXP"
	-@erase "$(OUTDIR)\HASHMD5.ILK"
	-@erase "$(OUTDIR)\HASHMD5.pdb"
	-@erase "$(OUTDIR)\HASHMD5.PPL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HashMD5.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\HASHMD5.pdb" /debug /machine:I386 /out:"$(OUTDIR)\HASHMD5.PPL" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\hash.obj" \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_hash_md5.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\HASHMD5.PPL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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
!IF EXISTS("HashMD5.dep")
!INCLUDE "HashMD5.dep"
!ELSE 
!MESSAGE Warning: cannot find "HashMD5.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HashMD5 - Win32 ReleaseWithTrace" || "$(CFG)" == "HashMD5 - Win32 Release" || "$(CFG)" == "HashMD5 - Win32 Debug"
SOURCE=.\hash.c

"$(INTDIR)\hash.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\HASH_API\MD5\hash_md5.c

"$(INTDIR)\hash_md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\HASH_API\MD5\md5.cpp

"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_hash_md5.c

"$(INTDIR)\plugin_hash_md5.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\resource.rc

"$(INTDIR)\resource.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


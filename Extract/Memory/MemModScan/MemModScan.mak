# Microsoft Developer Studio Generated NMAKE File, Based on MemModScan.dsp
!IF "$(CFG)" == ""
CFG=MemModScan - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MemModScan - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MemModScan - Win32 Release" && "$(CFG)" != "MemModScan - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MemModScan.mak" CFG="MemModScan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MemModScan - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MemModScan - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MemModScan - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/MemModScan
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\MemModSc.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\plugin_memmodscan.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MemModSc.exp"
	-@erase "$(OUTDIR)\MemModSc.pdb"
	-@erase "$(OUTDIR)\MemModSc.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MEMMODSCAN_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MemModScan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x65400000" /dll /incremental:no /pdb:"$(OUTDIR)\MemModSc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MemModSc.ppl" /implib:"$(OUTDIR)\MemModSc.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\plugin_memmodscan.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\MemModSc.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\MemModSc.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MemModSc.ppl"
   tsigner "\out\Release\MemModSc.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MemModScan - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/MemModScan
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\MemModSc.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\plugin_memmodscan.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MemModSc.exp"
	-@erase "$(OUTDIR)\MemModSc.ilk"
	-@erase "$(OUTDIR)\MemModSc.pdb"
	-@erase "$(OUTDIR)\MemModSc.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MEMMODSCAN_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MemModScan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MemModSc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MemModSc.ppl" /implib:"$(OUTDIR)\MemModSc.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\plugin_memmodscan.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\MemModSc.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MemModScan.dep")
!INCLUDE "MemModScan.dep"
!ELSE 
!MESSAGE Warning: cannot find "MemModScan.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MemModScan - Win32 Release" || "$(CFG)" == "MemModScan - Win32 Debug"
SOURCE=.\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\CommonFiles\Stuff\MemManag.c

"$(INTDIR)\MemManag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\objptr.c

"$(INTDIR)\objptr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os.c

"$(INTDIR)\os.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_memmodscan.c

"$(INTDIR)\plugin_memmodscan.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\template.rc

"$(INTDIR)\template.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


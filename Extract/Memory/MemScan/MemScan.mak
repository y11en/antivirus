# Microsoft Developer Studio Generated NMAKE File, Based on MemScan.dsp
!IF "$(CFG)" == ""
CFG=MemScan - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MemScan - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MemScan - Win32 Release" && "$(CFG)" != "MemScan - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MemScan.mak" CFG="MemScan - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MemScan - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MemScan - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MemScan - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/MemScan
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\MemScan.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\plugin_memmod.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MemScan.exp"
	-@erase "$(OUTDIR)\MemScan.pdb"
	-@erase "$(OUTDIR)\MemScan.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MEMSCAN_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MemScan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x65500000" /dll /incremental:no /pdb:"$(OUTDIR)\MemScan.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MemScan.ppl" /implib:"$(OUTDIR)\MemScan.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\plugin_memmod.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\MemScan.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\MemScan.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MemScan.ppl"
   tsigner "\out\Release\MemScan.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MemScan - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/MemScan
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\MemScan.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\MemManag.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\plugin_memmod.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MemScan.exp"
	-@erase "$(OUTDIR)\MemScan.ilk"
	-@erase "$(OUTDIR)\MemScan.pdb"
	-@erase "$(OUTDIR)\MemScan.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MEMSCAN_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MemScan.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MemScan.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MemScan.ppl" /implib:"$(OUTDIR)\MemScan.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\MemManag.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\plugin_memmod.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\MemScan.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MemScan.dep")
!INCLUDE "MemScan.dep"
!ELSE 
!MESSAGE Warning: cannot find "MemScan.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MemScan - Win32 Release" || "$(CFG)" == "MemScan - Win32 Debug"
SOURCE=.\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\CommonFiles\Stuff\MemManag.c

"$(INTDIR)\MemManag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\objptr.c

"$(INTDIR)\objptr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os.c

"$(INTDIR)\os.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_memmod.c

"$(INTDIR)\plugin_memmod.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\template.rc

"$(INTDIR)\template.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


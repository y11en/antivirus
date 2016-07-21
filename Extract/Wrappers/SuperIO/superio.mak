# Microsoft Developer Studio Generated NMAKE File, Based on superio.dsp
!IF "$(CFG)" == ""
CFG=superio - Win32 Debug
!MESSAGE No configuration specified. Defaulting to superio - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "superio - Win32 Release" && "$(CFG)" != "superio - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "superio.mak" CFG="superio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "superio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "superio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "superio - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/SuperIO
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\SuperIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_superio.obj"
	-@erase "$(INTDIR)\superio.obj"
	-@erase "$(INTDIR)\superio.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SuperIO.exp"
	-@erase "$(OUTDIR)\SuperIO.pdb"
	-@erase "$(OUTDIR)\SuperIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "superio_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\superio.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\superio.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x67e00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\SuperIO.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SuperIO.ppl" /implib:"$(OUTDIR)\SuperIO.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_superio.obj" \
	"$(INTDIR)\superio.obj" \
	"$(INTDIR)\superio.res"

"$(OUTDIR)\SuperIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\SuperIO.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\SuperIO.ppl"
   call prconvert "\out\Release\SuperIO.ppl"
	tsigner "\out\Release\SuperIO.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "superio - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/SuperIO
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\SuperIO.ppl" "$(OUTDIR)\superio.bsc"


CLEAN :
	-@erase "$(INTDIR)\plugin_superio.obj"
	-@erase "$(INTDIR)\plugin_superio.sbr"
	-@erase "$(INTDIR)\superio.obj"
	-@erase "$(INTDIR)\superio.res"
	-@erase "$(INTDIR)\superio.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\superio.bsc"
	-@erase "$(OUTDIR)\SuperIO.exp"
	-@erase "$(OUTDIR)\SuperIO.ilk"
	-@erase "$(OUTDIR)\SuperIO.pdb"
	-@erase "$(OUTDIR)\SuperIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "superio_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\superio.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\superio.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\plugin_superio.sbr" \
	"$(INTDIR)\superio.sbr"

"$(OUTDIR)\superio.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\SuperIO.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SuperIO.ppl" /implib:"$(OUTDIR)\SuperIO.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_superio.obj" \
	"$(INTDIR)\superio.obj" \
	"$(INTDIR)\superio.res"

"$(OUTDIR)\SuperIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("superio.dep")
!INCLUDE "superio.dep"
!ELSE 
!MESSAGE Warning: cannot find "superio.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "superio - Win32 Release" || "$(CFG)" == "superio - Win32 Debug"
SOURCE=.\plugin_superio.c

!IF  "$(CFG)" == "superio - Win32 Release"


"$(INTDIR)\plugin_superio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "superio - Win32 Debug"


"$(INTDIR)\plugin_superio.obj"	"$(INTDIR)\plugin_superio.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\superio.c

!IF  "$(CFG)" == "superio - Win32 Release"


"$(INTDIR)\superio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "superio - Win32 Debug"


"$(INTDIR)\superio.obj"	"$(INTDIR)\superio.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\superio.rc

"$(INTDIR)\superio.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


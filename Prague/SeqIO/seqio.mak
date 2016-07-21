# Microsoft Developer Studio Generated NMAKE File, Based on seqio.dsp
!IF "$(CFG)" == ""
CFG=seqio - Win32 Debug
!MESSAGE No configuration specified. Defaulting to seqio - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "seqio - Win32 Release" && "$(CFG)" != "seqio - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "seqio.mak" CFG="seqio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "seqio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "seqio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "seqio - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/SeqIO
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\prseqio.ppl"


CLEAN :
	-@erase "$(INTDIR)\seqio.obj"
	-@erase "$(INTDIR)\SeqIO.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prseqio.exp"
	-@erase "$(OUTDIR)\prseqio.pdb"
	-@erase "$(OUTDIR)\prseqio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SEQIO_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SeqIO.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\seqio.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x67600000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\prseqio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\prseqio.ppl" /implib:"$(OUTDIR)\prseqio.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\seqio.obj" \
	"$(INTDIR)\SeqIO.res"

"$(OUTDIR)\prseqio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\prseqio.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\prseqio.ppl"
   call prconvert "\out\Release\prseqio.ppl"
	tsigner "\out\Release\prseqio.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "seqio - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/SeqIO
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\prseqio.ppl"


CLEAN :
	-@erase "$(INTDIR)\seqio.obj"
	-@erase "$(INTDIR)\SeqIO.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prseqio.exp"
	-@erase "$(OUTDIR)\prseqio.ilk"
	-@erase "$(OUTDIR)\prseqio.pdb"
	-@erase "$(OUTDIR)\prseqio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SEQIO_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\SeqIO.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\seqio.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\prseqio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\prseqio.ppl" /implib:"$(OUTDIR)\prseqio.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\seqio.obj" \
	"$(INTDIR)\SeqIO.res"

"$(OUTDIR)\prseqio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("seqio.dep")
!INCLUDE "seqio.dep"
!ELSE 
!MESSAGE Warning: cannot find "seqio.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "seqio - Win32 Release" || "$(CFG)" == "seqio - Win32 Debug"
SOURCE=.\seqio.c

"$(INTDIR)\seqio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SeqIO.rc

"$(INTDIR)\SeqIO.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on buffer.dsp
!IF "$(CFG)" == ""
CFG=buffer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to buffer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "buffer - Win32 Release" && "$(CFG)" != "buffer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "buffer.mak" CFG="buffer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "buffer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "buffer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "buffer - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/buffer
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\buffer.ppl"


CLEAN :
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\buffer.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\buffer.exp"
	-@erase "$(OUTDIR)\buffer.pdb"
	-@erase "$(OUTDIR)\buffer.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "NDEBUG" /D "_USRDLL" /D "BUFFER_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\buffer.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\buffer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib /nologo /base:"0x62b00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\buffer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\buffer.ppl" /implib:"$(OUTDIR)\buffer.lib" /pdbtype:sept /libpath:"..\..\out\Release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\buffer.res"

"$(OUTDIR)\buffer.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\buffer.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\buffer.ppl"
   call prconvert "\out\Release\buffer.ppl"
	tsigner "\out\Release\buffer.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "buffer - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/buffer
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\buffer.ppl" "$(OUTDIR)\buffer.bsc"


CLEAN :
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\buffer.res"
	-@erase "$(INTDIR)\buffer.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\buffer.bsc"
	-@erase "$(OUTDIR)\buffer.exp"
	-@erase "$(OUTDIR)\buffer.ilk"
	-@erase "$(OUTDIR)\buffer.pdb"
	-@erase "$(OUTDIR)\buffer.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "_PRAGUE_TRACE_" /D "_DEBUG" /D "_USRDLL" /D "BUFFER_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\buffer.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\buffer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\buffer.sbr"

"$(OUTDIR)\buffer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=m_utils.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\buffer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\buffer.ppl" /implib:"$(OUTDIR)\buffer.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll" /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\buffer.res"

"$(OUTDIR)\buffer.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("buffer.dep")
!INCLUDE "buffer.dep"
!ELSE 
!MESSAGE Warning: cannot find "buffer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "buffer - Win32 Release" || "$(CFG)" == "buffer - Win32 Debug"
SOURCE=.\buffer.c

!IF  "$(CFG)" == "buffer - Win32 Release"


"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "buffer - Win32 Debug"


"$(INTDIR)\buffer.obj"	"$(INTDIR)\buffer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\buffer.rc

"$(INTDIR)\buffer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


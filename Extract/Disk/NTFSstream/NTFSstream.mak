# Microsoft Developer Studio Generated NMAKE File, Based on NTFSstream.dsp
!IF "$(CFG)" == ""
CFG=NTFSstream - Win32 Debug
!MESSAGE No configuration specified. Defaulting to NTFSstream - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NTFSstream - Win32 Release" && "$(CFG)" != "NTFSstream - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NTFSstream.mak" CFG="NTFSstream - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NTFSstream - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NTFSstream - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "NTFSstream - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/NTFSstream
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\NTFSstrm.ppl"


CLEAN :
	-@erase "$(INTDIR)\NTFSstream.res"
	-@erase "$(INTDIR)\plugin_ntfsstream.obj"
	-@erase "$(INTDIR)\stream_optr.obj"
	-@erase "$(INTDIR)\stream_util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\NTFSstrm.exp"
	-@erase "$(OUTDIR)\NTFSstrm.pdb"
	-@erase "$(OUTDIR)\NTFSstrm.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I ".." /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STREAM_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\NTFSstream.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NTFSstream.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /base:"0x65d00000" /dll /incremental:no /pdb:"$(OUTDIR)\NTFSstrm.pdb" /debug /machine:I386 /out:"$(OUTDIR)\NTFSstrm.ppl" /implib:"$(OUTDIR)\NTFSstrm.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_ntfsstream.obj" \
	"$(INTDIR)\stream_optr.obj" \
	"$(INTDIR)\stream_util.obj" \
	"$(INTDIR)\NTFSstream.res"

"$(OUTDIR)\NTFSstrm.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\NTFSstrm.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\NTFSstrm.ppl"
   tsigner "\out\Release\NTFSstrm.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "NTFSstream - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/NTFSstream
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\NTFSstrm.ppl" "$(OUTDIR)\NTFSstream.bsc"


CLEAN :
	-@erase "$(INTDIR)\NTFSstream.res"
	-@erase "$(INTDIR)\plugin_ntfsstream.obj"
	-@erase "$(INTDIR)\plugin_ntfsstream.sbr"
	-@erase "$(INTDIR)\stream_optr.obj"
	-@erase "$(INTDIR)\stream_optr.sbr"
	-@erase "$(INTDIR)\stream_util.obj"
	-@erase "$(INTDIR)\stream_util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\NTFSstream.bsc"
	-@erase "$(OUTDIR)\NTFSstrm.exp"
	-@erase "$(OUTDIR)\NTFSstrm.ilk"
	-@erase "$(OUTDIR)\NTFSstrm.pdb"
	-@erase "$(OUTDIR)\NTFSstrm.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STREAM_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\NTFSstream.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\NTFSstream.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\plugin_ntfsstream.sbr" \
	"$(INTDIR)\stream_optr.sbr" \
	"$(INTDIR)\stream_util.sbr"

"$(OUTDIR)\NTFSstream.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\NTFSstrm.pdb" /debug /machine:I386 /out:"$(OUTDIR)\NTFSstrm.ppl" /implib:"$(OUTDIR)\NTFSstrm.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_ntfsstream.obj" \
	"$(INTDIR)\stream_optr.obj" \
	"$(INTDIR)\stream_util.obj" \
	"$(INTDIR)\NTFSstream.res"

"$(OUTDIR)\NTFSstrm.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("NTFSstream.dep")
!INCLUDE "NTFSstream.dep"
!ELSE 
!MESSAGE Warning: cannot find "NTFSstream.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "NTFSstream - Win32 Release" || "$(CFG)" == "NTFSstream - Win32 Debug"
SOURCE=.\plugin_ntfsstream.c

!IF  "$(CFG)" == "NTFSstream - Win32 Release"


"$(INTDIR)\plugin_ntfsstream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "NTFSstream - Win32 Debug"


"$(INTDIR)\plugin_ntfsstream.obj"	"$(INTDIR)\plugin_ntfsstream.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stream_optr.c

!IF  "$(CFG)" == "NTFSstream - Win32 Release"


"$(INTDIR)\stream_optr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "NTFSstream - Win32 Debug"


"$(INTDIR)\stream_optr.obj"	"$(INTDIR)\stream_optr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stream_util.c

!IF  "$(CFG)" == "NTFSstream - Win32 Release"


"$(INTDIR)\stream_util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "NTFSstream - Win32 Debug"


"$(INTDIR)\stream_util.obj"	"$(INTDIR)\stream_util.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\NTFSstream.rc

"$(INTDIR)\NTFSstream.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


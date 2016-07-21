# Microsoft Developer Studio Generated NMAKE File, Based on avlib.dsp
!IF "$(CFG)" == ""
CFG=avlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to avlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "avlib - Win32 Release" && "$(CFG)" != "avlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avlib.mak" CFG="avlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avlib - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/AvLib
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\avlib.ppl"


CLEAN :
	-@erase "$(INTDIR)\avlib.obj"
	-@erase "$(INTDIR)\avlib.res"
	-@erase "$(INTDIR)\isarcbuf.obj"
	-@erase "$(INTDIR)\isexec.obj"
	-@erase "$(INTDIR)\isexecbuff.obj"
	-@erase "$(INTDIR)\psw_heuristic.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avlib.exp"
	-@erase "$(OUTDIR)\avlib.pdb"
	-@erase "$(OUTDIR)\avlib.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVLIB_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avlib.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x61b00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\avlib.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avlib.ppl" /implib:"$(OUTDIR)\avlib.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\avlib.obj" \
	"$(INTDIR)\isarcbuf.obj" \
	"$(INTDIR)\isexec.obj" \
	"$(INTDIR)\isexecbuff.obj" \
	"$(INTDIR)\psw_heuristic.obj" \
	"$(INTDIR)\avlib.res"

"$(OUTDIR)\avlib.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\avlib.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\avlib.ppl"
   call prconvert "\out\Release\avlib.ppl"
	tsigner "\out\Release\avlib.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "avlib - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/AvLib
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\avlib.ppl" "$(OUTDIR)\avlib.bsc"


CLEAN :
	-@erase "$(INTDIR)\avlib.obj"
	-@erase "$(INTDIR)\avlib.res"
	-@erase "$(INTDIR)\avlib.sbr"
	-@erase "$(INTDIR)\isarcbuf.obj"
	-@erase "$(INTDIR)\isarcbuf.sbr"
	-@erase "$(INTDIR)\isexec.obj"
	-@erase "$(INTDIR)\isexec.sbr"
	-@erase "$(INTDIR)\isexecbuff.obj"
	-@erase "$(INTDIR)\isexecbuff.sbr"
	-@erase "$(INTDIR)\psw_heuristic.obj"
	-@erase "$(INTDIR)\psw_heuristic.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avlib.bsc"
	-@erase "$(OUTDIR)\avlib.exp"
	-@erase "$(OUTDIR)\avlib.ilk"
	-@erase "$(OUTDIR)\avlib.pdb"
	-@erase "$(OUTDIR)\avlib.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVLIB_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avlib.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avlib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\avlib.sbr" \
	"$(INTDIR)\isarcbuf.sbr" \
	"$(INTDIR)\isexec.sbr" \
	"$(INTDIR)\isexecbuff.sbr" \
	"$(INTDIR)\psw_heuristic.sbr"

"$(OUTDIR)\avlib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\avlib.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avlib.ppl" /implib:"$(OUTDIR)\avlib.lib" /pdbtype:sept /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\avlib.obj" \
	"$(INTDIR)\isarcbuf.obj" \
	"$(INTDIR)\isexec.obj" \
	"$(INTDIR)\isexecbuff.obj" \
	"$(INTDIR)\psw_heuristic.obj" \
	"$(INTDIR)\avlib.res"

"$(OUTDIR)\avlib.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("avlib.dep")
!INCLUDE "avlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "avlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avlib - Win32 Release" || "$(CFG)" == "avlib - Win32 Debug"
SOURCE=.\avlib.c

!IF  "$(CFG)" == "avlib - Win32 Release"


"$(INTDIR)\avlib.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avlib - Win32 Debug"


"$(INTDIR)\avlib.obj"	"$(INTDIR)\avlib.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\isarcbuf.c

!IF  "$(CFG)" == "avlib - Win32 Release"


"$(INTDIR)\isarcbuf.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avlib - Win32 Debug"


"$(INTDIR)\isarcbuf.obj"	"$(INTDIR)\isarcbuf.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\isexec.c

!IF  "$(CFG)" == "avlib - Win32 Release"


"$(INTDIR)\isexec.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avlib - Win32 Debug"


"$(INTDIR)\isexec.obj"	"$(INTDIR)\isexec.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\isexecbuff.c

!IF  "$(CFG)" == "avlib - Win32 Release"


"$(INTDIR)\isexecbuff.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avlib - Win32 Debug"


"$(INTDIR)\isexecbuff.obj"	"$(INTDIR)\isexecbuff.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\avpmgr\psw_heuristic.c

!IF  "$(CFG)" == "avlib - Win32 Release"


"$(INTDIR)\psw_heuristic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "avlib - Win32 Debug"


"$(INTDIR)\psw_heuristic.obj"	"$(INTDIR)\psw_heuristic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\avlib.rc

"$(INTDIR)\avlib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


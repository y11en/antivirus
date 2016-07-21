# Microsoft Developer Studio Generated NMAKE File, Based on thpoolimp.dsp
!IF "$(CFG)" == ""
CFG=thpoolimp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to thpoolimp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "thpoolimp - Win32 Release" && "$(CFG)" != "thpoolimp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "thpoolimp.mak" CFG="thpoolimp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "thpoolimp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "thpoolimp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "thpoolimp - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/thpoolimp
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\thpimpl.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_thpoolimp.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\thpimpl_win32.obj"
	-@erase "$(INTDIR)\thpoolimp.res"
	-@erase "$(INTDIR)\threadpool.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\thpimpl.exp"
	-@erase "$(OUTDIR)\thpimpl.pdb"
	-@erase "$(OUTDIR)\thpimpl.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "thpoolimp_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\thpoolimp.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thpoolimp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib User32.lib FSSync.lib advapi32.lib /nologo /base:"0x68000000" /dll /incremental:no /pdb:"$(OUTDIR)\thpimpl.pdb" /debug /machine:I386 /out:"$(OUTDIR)\thpimpl.ppl" /implib:"$(OUTDIR)\thpimpl.lib" /pdbtype:sept /libpath:"../../out/Release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_thpoolimp.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\thpimpl_win32.obj" \
	"$(INTDIR)\threadpool.obj" \
	"$(INTDIR)\thpoolimp.res"

"$(OUTDIR)\thpimpl.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\thpimpl.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\thpimpl.ppl"
   tsigner "\out\Release\thpimpl.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "thpoolimp - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/thpoolimp
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\thpimpl.ppl" "$(OUTDIR)\thpoolimp.bsc"


CLEAN :
	-@erase "$(INTDIR)\plugin_thpoolimp.obj"
	-@erase "$(INTDIR)\plugin_thpoolimp.sbr"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sa.sbr"
	-@erase "$(INTDIR)\thpimpl_win32.obj"
	-@erase "$(INTDIR)\thpimpl_win32.sbr"
	-@erase "$(INTDIR)\thpoolimp.res"
	-@erase "$(INTDIR)\threadpool.obj"
	-@erase "$(INTDIR)\threadpool.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\thpimpl.exp"
	-@erase "$(OUTDIR)\thpimpl.ilk"
	-@erase "$(OUTDIR)\thpimpl.pdb"
	-@erase "$(OUTDIR)\thpimpl.ppl"
	-@erase "$(OUTDIR)\thpoolimp.bsc"
	-@erase "$(OUTDIR)\thpoolimp.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "thpoolimp_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\thpoolimp.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\thpoolimp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\plugin_thpoolimp.sbr" \
	"$(INTDIR)\sa.sbr" \
	"$(INTDIR)\thpimpl_win32.sbr" \
	"$(INTDIR)\threadpool.sbr"

"$(OUTDIR)\thpoolimp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib User32.lib FSSync.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\thpimpl.pdb" /map:"../../out/Debug/thpoolimp.map" /debug /machine:I386 /out:"$(OUTDIR)\thpimpl.ppl" /implib:"$(OUTDIR)\thpimpl.lib" /pdbtype:sept /libpath:"../../out/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_thpoolimp.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\thpimpl_win32.obj" \
	"$(INTDIR)\threadpool.obj" \
	"$(INTDIR)\thpoolimp.res"

"$(OUTDIR)\thpimpl.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("thpoolimp.dep")
!INCLUDE "thpoolimp.dep"
!ELSE 
!MESSAGE Warning: cannot find "thpoolimp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "thpoolimp - Win32 Release" || "$(CFG)" == "thpoolimp - Win32 Debug"
SOURCE=.\plugin_thpoolimp.cpp

!IF  "$(CFG)" == "thpoolimp - Win32 Release"


"$(INTDIR)\plugin_thpoolimp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "thpoolimp - Win32 Debug"


"$(INTDIR)\plugin_thpoolimp.obj"	"$(INTDIR)\plugin_thpoolimp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\CommonFiles\Service\sa.cpp

!IF  "$(CFG)" == "thpoolimp - Win32 Release"


"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "thpoolimp - Win32 Debug"


"$(INTDIR)\sa.obj"	"$(INTDIR)\sa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\thpimpl_win32.cpp

!IF  "$(CFG)" == "thpoolimp - Win32 Release"


"$(INTDIR)\thpimpl_win32.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "thpoolimp - Win32 Debug"


"$(INTDIR)\thpimpl_win32.obj"	"$(INTDIR)\thpimpl_win32.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\threadpool.cpp

!IF  "$(CFG)" == "thpoolimp - Win32 Release"


"$(INTDIR)\threadpool.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "thpoolimp - Win32 Debug"


"$(INTDIR)\threadpool.obj"	"$(INTDIR)\threadpool.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\thpoolimp.rc

"$(INTDIR)\thpoolimp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


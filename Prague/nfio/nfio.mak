# Microsoft Developer Studio Generated NMAKE File, Based on nfio.dsp
!IF "$(CFG)" == ""
CFG=nfio - Win32 Debug
!MESSAGE No configuration specified. Defaulting to nfio - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "nfio - Win32 Release" && "$(CFG)" != "nfio - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nfio.mak" CFG="nfio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nfio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nfio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "nfio - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/nfio
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\nfio.ppl"


CLEAN :
	-@erase "$(INTDIR)\nfio.res"
	-@erase "$(INTDIR)\plugin_win32_nfio.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32_io.obj"
	-@erase "$(INTDIR)\win32_objptr.obj"
	-@erase "$(INTDIR)\win32_os.obj"
	-@erase "$(OUTDIR)\nfio.exp"
	-@erase "$(OUTDIR)\nfio.pdb"
	-@erase "$(OUTDIR)\nfio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nfio.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nfio.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib Advapi32.lib /nologo /base:"0x65b00000" /dll /incremental:no /pdb:"$(OUTDIR)\nfio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\nfio.ppl" /implib:"$(OUTDIR)\nfio.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_win32_nfio.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\win32_io.obj" \
	"$(INTDIR)\win32_objptr.obj" \
	"$(INTDIR)\win32_os.obj" \
	"$(INTDIR)\nfio.res"

"$(OUTDIR)\nfio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\nfio.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\nfio.ppl"
   tsigner "\out\Release\nfio.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/nfio
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\nfio.ppl" "$(OUTDIR)\nfio.bsc"


CLEAN :
	-@erase "$(INTDIR)\nfio.res"
	-@erase "$(INTDIR)\plugin_win32_nfio.obj"
	-@erase "$(INTDIR)\plugin_win32_nfio.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\win32_io.obj"
	-@erase "$(INTDIR)\win32_io.sbr"
	-@erase "$(INTDIR)\win32_objptr.obj"
	-@erase "$(INTDIR)\win32_objptr.sbr"
	-@erase "$(INTDIR)\win32_os.obj"
	-@erase "$(INTDIR)\win32_os.sbr"
	-@erase "$(OUTDIR)\nfio.bsc"
	-@erase "$(OUTDIR)\nfio.exp"
	-@erase "$(OUTDIR)\nfio.ilk"
	-@erase "$(OUTDIR)\nfio.map"
	-@erase "$(OUTDIR)\nfio.pdb"
	-@erase "$(OUTDIR)\nfio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\nfio.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nfio.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\plugin_win32_nfio.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\win32_io.sbr" \
	"$(INTDIR)\win32_objptr.sbr" \
	"$(INTDIR)\win32_os.sbr"

"$(OUTDIR)\nfio.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib Advapi32.lib /nologo /base:"0xAD700000" /dll /incremental:yes /pdb:"$(OUTDIR)\nfio.pdb" /map:"../../out/Debug/nfio.map" /debug /machine:I386 /out:"$(OUTDIR)\nfio.ppl" /implib:"$(OUTDIR)\nfio.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_win32_nfio.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\win32_io.obj" \
	"$(INTDIR)\win32_objptr.obj" \
	"$(INTDIR)\win32_os.obj" \
	"$(INTDIR)\nfio.res"

"$(OUTDIR)\nfio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("nfio.dep")
!INCLUDE "nfio.dep"
!ELSE 
!MESSAGE Warning: cannot find "nfio.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "nfio - Win32 Release" || "$(CFG)" == "nfio - Win32 Debug"
SOURCE=.\plugin_win32_nfio.c

!IF  "$(CFG)" == "nfio - Win32 Release"


"$(INTDIR)\plugin_win32_nfio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"


"$(INTDIR)\plugin_win32_nfio.obj"	"$(INTDIR)\plugin_win32_nfio.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\util.c

!IF  "$(CFG)" == "nfio - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32_io.cpp

!IF  "$(CFG)" == "nfio - Win32 Release"


"$(INTDIR)\win32_io.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"


"$(INTDIR)\win32_io.obj"	"$(INTDIR)\win32_io.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32_objptr.c

!IF  "$(CFG)" == "nfio - Win32 Release"


"$(INTDIR)\win32_objptr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"


"$(INTDIR)\win32_objptr.obj"	"$(INTDIR)\win32_objptr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32_os.c

!IF  "$(CFG)" == "nfio - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\win32_os.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "nfio - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NFIO_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\win32_os.obj"	"$(INTDIR)\win32_os.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\nfio.rc

"$(INTDIR)\nfio.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


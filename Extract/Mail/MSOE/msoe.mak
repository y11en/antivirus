# Microsoft Developer Studio Generated NMAKE File, Based on msoe.dsp
!IF "$(CFG)" == ""
CFG=msoe - Win32 Debug
!MESSAGE No configuration specified. Defaulting to msoe - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msoe - Win32 Release" && "$(CFG)" != "msoe - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "msoe.mak" CFG="msoe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msoe - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msoe - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "msoe - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/MSOE
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\msoe.ppl"


CLEAN :
	-@erase "$(INTDIR)\hook.obj"
	-@erase "$(INTDIR)\msoe.obj"
	-@erase "$(INTDIR)\msoe.res"
	-@erase "$(INTDIR)\msoe_base.obj"
	-@erase "$(INTDIR)\msoe_io.obj"
	-@erase "$(INTDIR)\msoe_objptr.obj"
	-@erase "$(INTDIR)\msoe_os.obj"
	-@erase "$(INTDIR)\plugin_msoe.obj"
	-@erase "$(INTDIR)\stream.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\msoe.exp"
	-@erase "$(OUTDIR)\msoe.pdb"
	-@erase "$(OUTDIR)\msoe.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include\iface" /I "..\include" /I ".\MSOEAPI" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "msoe_EXPORTS" /D "C_STYLE_PROP" /D "_USE_VTBL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\msoe.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msoe.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib mapi32.lib ole32.lib user32.lib advapi32.lib /nologo /base:"0x65700000" /dll /incremental:no /pdb:"$(OUTDIR)\msoe.pdb" /debug /machine:I386 /out:"$(OUTDIR)\msoe.ppl" /implib:"$(OUTDIR)\msoe.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /IGNORE:4089 /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\hook.obj" \
	"$(INTDIR)\msoe.obj" \
	"$(INTDIR)\msoe_base.obj" \
	"$(INTDIR)\msoe_io.obj" \
	"$(INTDIR)\msoe_objptr.obj" \
	"$(INTDIR)\msoe_os.obj" \
	"$(INTDIR)\plugin_msoe.obj" \
	"$(INTDIR)\stream.obj" \
	"$(INTDIR)\msoe.res"

"$(OUTDIR)\msoe.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\msoe.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\msoe.ppl"
   tsigner "\out\Release\msoe.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/MSOE
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\msoe.ppl" "$(OUTDIR)\msoe.bsc"


CLEAN :
	-@erase "$(INTDIR)\hook.obj"
	-@erase "$(INTDIR)\hook.sbr"
	-@erase "$(INTDIR)\msoe.obj"
	-@erase "$(INTDIR)\msoe.res"
	-@erase "$(INTDIR)\msoe.sbr"
	-@erase "$(INTDIR)\msoe_base.obj"
	-@erase "$(INTDIR)\msoe_base.sbr"
	-@erase "$(INTDIR)\msoe_io.obj"
	-@erase "$(INTDIR)\msoe_io.sbr"
	-@erase "$(INTDIR)\msoe_objptr.obj"
	-@erase "$(INTDIR)\msoe_objptr.sbr"
	-@erase "$(INTDIR)\msoe_os.obj"
	-@erase "$(INTDIR)\msoe_os.sbr"
	-@erase "$(INTDIR)\plugin_msoe.obj"
	-@erase "$(INTDIR)\plugin_msoe.sbr"
	-@erase "$(INTDIR)\stream.obj"
	-@erase "$(INTDIR)\stream.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\msoe.bsc"
	-@erase "$(OUTDIR)\msoe.exp"
	-@erase "$(OUTDIR)\msoe.ilk"
	-@erase "$(OUTDIR)\msoe.map"
	-@erase "$(OUTDIR)\msoe.pdb"
	-@erase "$(OUTDIR)\msoe.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include\iface" /I "..\include" /I ".\MSOEAPI" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "msoe_EXPORTS" /D "C_STYLE_PROP" /D "_USE_VTBL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\msoe.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msoe.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\hook.sbr" \
	"$(INTDIR)\msoe.sbr" \
	"$(INTDIR)\msoe_base.sbr" \
	"$(INTDIR)\msoe_io.sbr" \
	"$(INTDIR)\msoe_objptr.sbr" \
	"$(INTDIR)\msoe_os.sbr" \
	"$(INTDIR)\plugin_msoe.sbr" \
	"$(INTDIR)\stream.sbr"

"$(OUTDIR)\msoe.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib mapi32.lib ole32.lib user32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\msoe.pdb" /map:"../../out/Debug/msoe.map" /debug /machine:I386 /out:"$(OUTDIR)\msoe.ppl" /implib:"$(OUTDIR)\msoe.lib" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\hook.obj" \
	"$(INTDIR)\msoe.obj" \
	"$(INTDIR)\msoe_base.obj" \
	"$(INTDIR)\msoe_io.obj" \
	"$(INTDIR)\msoe_objptr.obj" \
	"$(INTDIR)\msoe_os.obj" \
	"$(INTDIR)\plugin_msoe.obj" \
	"$(INTDIR)\stream.obj" \
	"$(INTDIR)\msoe.res"

"$(OUTDIR)\msoe.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("msoe.dep")
!INCLUDE "msoe.dep"
!ELSE 
!MESSAGE Warning: cannot find "msoe.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "msoe - Win32 Release" || "$(CFG)" == "msoe - Win32 Debug"
SOURCE=.\hook.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\hook.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\hook.obj"	"$(INTDIR)\hook.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msoe.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\msoe.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\msoe.obj"	"$(INTDIR)\msoe.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msoe_base.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\msoe_base.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\msoe_base.obj"	"$(INTDIR)\msoe_base.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msoe_io.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\msoe_io.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\msoe_io.obj"	"$(INTDIR)\msoe_io.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msoe_objptr.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\msoe_objptr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\msoe_objptr.obj"	"$(INTDIR)\msoe_objptr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msoe_os.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\msoe_os.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\msoe_os.obj"	"$(INTDIR)\msoe_os.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_msoe.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\plugin_msoe.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\plugin_msoe.obj"	"$(INTDIR)\plugin_msoe.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stream.cpp

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\stream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\stream.obj"	"$(INTDIR)\stream.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\msoe.rc

!IF  "$(CFG)" == "msoe - Win32 Release"


"$(INTDIR)\msoe.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\msoe.res" /i "..\..\CommonFiles" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "msoe - Win32 Debug"


"$(INTDIR)\msoe.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\msoe.res" /i "..\..\CommonFiles" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 


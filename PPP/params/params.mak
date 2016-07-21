# Microsoft Developer Studio Generated NMAKE File, Based on params.dsp
!IF "$(CFG)" == ""
CFG=params - Win32 Debug
!MESSAGE No configuration specified. Defaulting to params - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "params - Win32 Release" && "$(CFG)" != "params - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "params.mak" CFG="params - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "params - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "params - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "params - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/params
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\params.ppl"

!ELSE 

ALL : "stream - Win32 Release" "$(OUTDIR)\params.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"stream - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\params.res"
	-@erase "$(INTDIR)\plugin_params.obj"
	-@erase "$(INTDIR)\startup.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\params.exp"
	-@erase "$(OUTDIR)\params.pdb"
	-@erase "$(OUTDIR)\params.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "./rpc" /I "../include" /I "../include/iface" /I "../../prague" /I "../../prague/include" /I "./" /I "../../prague/include/iface" /I "../../updater60/Include" /I "../../CommonFiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "params_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\params.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\params.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=stream.lib rpcrt4.lib /nologo /base:"0x66200000" /entry:"_DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\params.pdb" /debug /machine:I386 /out:"$(OUTDIR)\params.ppl" /implib:"$(OUTDIR)\params.lib" /pdbtype:sept /libpath:"../../CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_params.obj" \
	"$(INTDIR)\startup.obj" \
	"$(INTDIR)\params.res" \
	"$(OUTDIR)\stream.lib"

"$(OUTDIR)\params.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\params.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "stream - Win32 Release" "$(OUTDIR)\params.ppl"
   call prconvert "\out\Release\params.ppl"
	call tsigner \out\Release\params.ppl
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "params - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/params
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\params.ppl" "$(OUTDIR)\params.bsc"

!ELSE 

ALL : "stream - Win32 Debug" "$(OUTDIR)\params.ppl" "$(OUTDIR)\params.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"stream - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\params.res"
	-@erase "$(INTDIR)\plugin_params.obj"
	-@erase "$(INTDIR)\plugin_params.sbr"
	-@erase "$(INTDIR)\startup.obj"
	-@erase "$(INTDIR)\startup.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\params.bsc"
	-@erase "$(OUTDIR)\params.exp"
	-@erase "$(OUTDIR)\params.ilk"
	-@erase "$(OUTDIR)\params.pdb"
	-@erase "$(OUTDIR)\params.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./rpc" /I "../include" /I "../include/iface" /I "../../prague" /I "../../prague/include" /I "./" /I "../../prague/include/iface" /I "../../updater60/Include" /I "../../CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "params_EXPORTS" /D "C_STYLE_PROP" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\params.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\params.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\plugin_params.sbr" \
	"$(INTDIR)\startup.sbr"

"$(OUTDIR)\params.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=stream.lib rpcrt4.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\params.pdb" /debug /machine:I386 /out:"$(OUTDIR)\params.ppl" /implib:"$(OUTDIR)\params.lib" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_params.obj" \
	"$(INTDIR)\startup.obj" \
	"$(INTDIR)\params.res" \
	"$(OUTDIR)\stream.lib"

"$(OUTDIR)\params.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("params.dep")
!INCLUDE "params.dep"
!ELSE 
!MESSAGE Warning: cannot find "params.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "params - Win32 Release" || "$(CFG)" == "params - Win32 Debug"
SOURCE=.\plugin_params.cpp

!IF  "$(CFG)" == "params - Win32 Release"


"$(INTDIR)\plugin_params.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "params - Win32 Debug"


"$(INTDIR)\plugin_params.obj"	"$(INTDIR)\plugin_params.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\startup.c

!IF  "$(CFG)" == "params - Win32 Release"


"$(INTDIR)\startup.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "params - Win32 Debug"


"$(INTDIR)\startup.obj"	"$(INTDIR)\startup.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\params.rc

"$(INTDIR)\params.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "params - Win32 Release"

"stream - Win32 Release" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" 
   cd "..\..\PPP\params"

"stream - Win32 ReleaseCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\params"

!ELSEIF  "$(CFG)" == "params - Win32 Debug"

"stream - Win32 Debug" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" 
   cd "..\..\PPP\params"

"stream - Win32 DebugCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\params"

!ENDIF 


!ENDIF 


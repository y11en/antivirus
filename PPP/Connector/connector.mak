# Microsoft Developer Studio Generated NMAKE File, Based on connector.dsp
!IF "$(CFG)" == ""
CFG=CONNECTOR - WIN32 DEBUG
!MESSAGE No configuration specified. Defaulting to CONNECTOR - WIN32 DEBUG.
!ENDIF 

!IF "$(CFG)" != "connector - Win32 Release" && "$(CFG)" != "connector - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "connector.mak" CFG="CONNECTOR - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "connector - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "connector - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "connector - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/connector
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\connector.dll"


CLEAN :
	-@erase "$(INTDIR)\cnt_csc.obj"
	-@erase "$(INTDIR)\cnt_prague.obj"
	-@erase "$(INTDIR)\connector.obj"
	-@erase "$(INTDIR)\connector.res"
	-@erase "$(INTDIR)\Converter.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\connector.dll"
	-@erase "$(OUTDIR)\connector.exp"
	-@erase "$(OUTDIR)\connector.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\..\Prague\Include" /I "..\Include" /I "..\CSShare" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /D "NOT_PRAGUE_TRACE_" /D "connector_BUILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\connector.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\connector.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=pr_remote.lib kltrace.lib klcsc.lib /nologo /base:"0x62f00000" /dll /incremental:no /pdb:"$(OUTDIR)\connector.pdb" /debug /machine:I386 /out:"$(OUTDIR)\connector.dll" /implib:"$(OUTDIR)\connector.lib" /pdbtype:sept /libpath:"..\..\out\release" /libpath:"..\csshare\lib\release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\cnt_csc.obj" \
	"$(INTDIR)\cnt_prague.obj" \
	"$(INTDIR)\connector.obj" \
	"$(INTDIR)\Converter.obj" \
	"$(INTDIR)\connector.res"

"$(OUTDIR)\connector.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\connector.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\connector.dll"
   tsigner "\out\Release\connector.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "connector - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/connector
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\connector.dll" "$(OUTDIR)\connector.bsc"


CLEAN :
	-@erase "$(INTDIR)\cnt_csc.obj"
	-@erase "$(INTDIR)\cnt_csc.sbr"
	-@erase "$(INTDIR)\cnt_prague.obj"
	-@erase "$(INTDIR)\cnt_prague.sbr"
	-@erase "$(INTDIR)\connector.obj"
	-@erase "$(INTDIR)\connector.res"
	-@erase "$(INTDIR)\connector.sbr"
	-@erase "$(INTDIR)\Converter.obj"
	-@erase "$(INTDIR)\Converter.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\connector.bsc"
	-@erase "$(OUTDIR)\connector.dll"
	-@erase "$(OUTDIR)\connector.exp"
	-@erase "$(OUTDIR)\connector.ilk"
	-@erase "$(OUTDIR)\connector.lib"
	-@erase "$(OUTDIR)\connector.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\CommonFiles" /I "..\..\Prague\Include" /I "..\Include" /I "..\CSShare" /D "NOT_VERIFY" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /D "_PRAGUE_TRACE_" /D "connector_BUILD" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\connector.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\connector.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cnt_csc.sbr" \
	"$(INTDIR)\cnt_prague.sbr" \
	"$(INTDIR)\connector.sbr" \
	"$(INTDIR)\Converter.sbr"

"$(OUTDIR)\connector.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=pr_remote.lib kltrace.lib klcsc.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\connector.pdb" /debug /machine:I386 /out:"$(OUTDIR)\connector.dll" /implib:"$(OUTDIR)\connector.lib" /pdbtype:sept /libpath:"..\..\out\debug" /libpath:"..\csshare\lib\debug" 
LINK32_OBJS= \
	"$(INTDIR)\cnt_csc.obj" \
	"$(INTDIR)\cnt_prague.obj" \
	"$(INTDIR)\connector.obj" \
	"$(INTDIR)\Converter.obj" \
	"$(INTDIR)\connector.res"

"$(OUTDIR)\connector.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("connector.dep")
!INCLUDE "connector.dep"
!ELSE 
!MESSAGE Warning: cannot find "connector.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "connector - Win32 Release" || "$(CFG)" == "connector - Win32 Debug"
SOURCE=.\cnt_csc.cpp

!IF  "$(CFG)" == "connector - Win32 Release"


"$(INTDIR)\cnt_csc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "connector - Win32 Debug"


"$(INTDIR)\cnt_csc.obj"	"$(INTDIR)\cnt_csc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\cnt_prague.cpp

!IF  "$(CFG)" == "connector - Win32 Release"


"$(INTDIR)\cnt_prague.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "connector - Win32 Debug"


"$(INTDIR)\cnt_prague.obj"	"$(INTDIR)\cnt_prague.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\connector.cpp

!IF  "$(CFG)" == "connector - Win32 Release"


"$(INTDIR)\connector.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "connector - Win32 Debug"


"$(INTDIR)\connector.obj"	"$(INTDIR)\connector.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\connector.rc

"$(INTDIR)\connector.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\Include\common\Converter.cpp

!IF  "$(CFG)" == "connector - Win32 Release"


"$(INTDIR)\Converter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "connector - Win32 Debug"


"$(INTDIR)\Converter.obj"	"$(INTDIR)\Converter.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on avp3info.dsp
!IF "$(CFG)" == ""
CFG=AVP3Info - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AVP3Info - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AVP3Info - Win32 Release" && "$(CFG)" != "AVP3Info - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avp3info.mak" CFG="AVP3Info - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AVP3Info - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AVP3Info - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AVP3Info - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\out\Release\avp3info.ppl"


CLEAN :
	-@erase "$(INTDIR)\avp3info.obj"
	-@erase "$(INTDIR)\avp3info_res.res"
	-@erase "$(INTDIR)\plugin_avp3info.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avp3info.exp"
	-@erase "..\..\out\Release\avp3info.pdb"
	-@erase "..\..\out\Release\avp3info.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "../Include" /I "../../COMMONFILES" /I "../../COMMONFILES/STUFF" /I "../" /I "../../AVP32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\avp3info_res.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avp3info.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib /nologo /base:"0x61e00000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"..\..\out/Release/avp3info.pdb" /debug /machine:I386 /nodefaultlib /out:"..\..\out/Release/avp3info.ppl" /implib:"$(OUTDIR)\avp3info.lib" /pdbtype:sept /IGNORE:4098 /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\avp3info.obj" \
	"$(INTDIR)\plugin_avp3info.obj" \
	"$(INTDIR)\avp3info_res.res"

"..\..\out\Release\avp3info.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\avp3info.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\out\Release\avp3info.ppl"
   call prconvert "\out\Release\avp3info.ppl"
	tsigner "\out\Release\avp3info.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "AVP3Info - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\out\Debug\avp3info.ppl"


CLEAN :
	-@erase "$(INTDIR)\avp3info.obj"
	-@erase "$(INTDIR)\avp3info_res.res"
	-@erase "$(INTDIR)\plugin_avp3info.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\out\Debug\avp3info.exp"
	-@erase "..\..\out\Debug\avp3info.ilk"
	-@erase "..\..\out\Debug\avp3info.pdb"
	-@erase "..\..\out\Debug\avp3info.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../Include" /I "../../COMMONFILES" /I "../../COMMONFILES/STUFF" /I "../" /I "../../AVP32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\avp3info_res.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avp3info.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"..\..\out/Debug/avp3info.pdb" /debug /machine:I386 /out:"..\..\out/Debug/avp3info.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\avp3info.obj" \
	"$(INTDIR)\plugin_avp3info.obj" \
	"$(INTDIR)\avp3info_res.res"

"..\..\out\Debug\avp3info.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("avp3info.dep")
!INCLUDE "avp3info.dep"
!ELSE 
!MESSAGE Warning: cannot find "avp3info.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AVP3Info - Win32 Release" || "$(CFG)" == "AVP3Info - Win32 Debug"
SOURCE=.\avp3info.cpp

"$(INTDIR)\avp3info.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_avp3info.cpp

"$(INTDIR)\plugin_avp3info.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\avp3info_res.rc"

"$(INTDIR)\avp3info_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


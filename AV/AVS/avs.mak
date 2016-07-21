# Microsoft Developer Studio Generated NMAKE File, Based on avs.dsp
!IF "$(CFG)" == ""
CFG=avs - Win32 Debug
!MESSAGE No configuration specified. Defaulting to avs - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "avs - Win32 Release" && "$(CFG)" != "avs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avs.mak" CFG="avs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avs - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/avs
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\avs.ppl"

!ELSE 

ALL : "FSDrvPlgn - Win32 Release" "avspm - Win32 Release" "$(OUTDIR)\avs.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"avspm - Win32 ReleaseCLEAN" "FSDrvPlgn - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\avs.obj"
	-@erase "$(INTDIR)\avs.res"
	-@erase "$(INTDIR)\avssession.obj"
	-@erase "$(INTDIR)\avstreats.obj"
	-@erase "$(INTDIR)\masks.obj"
	-@erase "$(INTDIR)\plugin_avs.obj"
	-@erase "$(INTDIR)\reciever.obj"
	-@erase "$(INTDIR)\scan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avs.exp"
	-@erase "$(OUTDIR)\avs.pdb"
	-@erase "$(OUTDIR)\avs.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "../include" /I "../../prague/include" /I "../../prague" /D "NDEBUG" /D "WIN32" /D "_PRAGUE_TRACE_" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avs.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib /nologo /base:"0x62200000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\avs.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\avs.ppl" /implib:"$(OUTDIR)\avs.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\avs.obj" \
	"$(INTDIR)\avssession.obj" \
	"$(INTDIR)\avstreats.obj" \
	"$(INTDIR)\masks.obj" \
	"$(INTDIR)\plugin_avs.obj" \
	"$(INTDIR)\reciever.obj" \
	"$(INTDIR)\scan.obj" \
	"$(INTDIR)\avs.res"

"$(OUTDIR)\avs.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\avs.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "FSDrvPlgn - Win32 Release" "avspm - Win32 Release" "$(OUTDIR)\avs.ppl"
   call prconvert "\out\Release\avs.ppl"
	tsigner "\out\Release\avs.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "avs - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/avs
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\avs.ppl"

!ELSE 

ALL : "FSDrvPlgn - Win32 Debug" "avspm - Win32 Debug" "$(OUTDIR)\avs.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"avspm - Win32 DebugCLEAN" "FSDrvPlgn - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\avs.obj"
	-@erase "$(INTDIR)\avs.res"
	-@erase "$(INTDIR)\avssession.obj"
	-@erase "$(INTDIR)\avstreats.obj"
	-@erase "$(INTDIR)\masks.obj"
	-@erase "$(INTDIR)\plugin_avs.obj"
	-@erase "$(INTDIR)\reciever.obj"
	-@erase "$(INTDIR)\scan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avs.exp"
	-@erase "$(OUTDIR)\avs.ilk"
	-@erase "$(OUTDIR)\avs.pdb"
	-@erase "$(OUTDIR)\avs.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../prague" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avs.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\avs.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avs.ppl" /implib:"$(OUTDIR)\avs.lib" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\avs.obj" \
	"$(INTDIR)\avssession.obj" \
	"$(INTDIR)\avstreats.obj" \
	"$(INTDIR)\masks.obj" \
	"$(INTDIR)\plugin_avs.obj" \
	"$(INTDIR)\reciever.obj" \
	"$(INTDIR)\scan.obj" \
	"$(INTDIR)\avs.res"

"$(OUTDIR)\avs.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("avs.dep")
!INCLUDE "avs.dep"
!ELSE 
!MESSAGE Warning: cannot find "avs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avs - Win32 Release" || "$(CFG)" == "avs - Win32 Debug"
SOURCE=.\avs.cpp

"$(INTDIR)\avs.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avssession.cpp

"$(INTDIR)\avssession.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avstreats.cpp

"$(INTDIR)\avstreats.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\masks.cpp

"$(INTDIR)\masks.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_avs.cpp

"$(INTDIR)\plugin_avs.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\reciever.cpp

"$(INTDIR)\reciever.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\scan.cpp

"$(INTDIR)\scan.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avs.rc

"$(INTDIR)\avs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "avs - Win32 Release"

"avspm - Win32 Release" : 
   cd ".\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Release" 
   cd ".."

"avspm - Win32 ReleaseCLEAN" : 
   cd ".\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "avs - Win32 Debug"

"avspm - Win32 Debug" : 
   cd ".\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Debug" 
   cd ".."

"avspm - Win32 DebugCLEAN" : 
   cd ".\PM"
   $(MAKE) /$(MAKEFLAGS) /F .\avspm.mak CFG="avspm - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "avs - Win32 Release"

"FSDrvPlgn - Win32 Release" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Release" 
   cd "..\..\PPP\AVS"

"FSDrvPlgn - Win32 ReleaseCLEAN" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\AVS"

!ELSEIF  "$(CFG)" == "avs - Win32 Debug"

"FSDrvPlgn - Win32 Debug" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Debug" 
   cd "..\..\PPP\AVS"

"FSDrvPlgn - Win32 DebugCLEAN" : 
   cd "\prague\fsdrvplgn"
   $(MAKE) /$(MAKEFLAGS) /F .\fsdrvplgn.mak CFG="FSDrvPlgn - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\AVS"

!ENDIF 


!ENDIF 


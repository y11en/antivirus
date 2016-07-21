# Microsoft Developer Studio Generated NMAKE File, Based on HashContainer.dsp
!IF "$(CFG)" == ""
CFG=HashContainer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to HashContainer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HashContainer - Win32 Release" && "$(CFG)" != "HashContainer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HashContainer.mak" CFG="HashContainer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HashContainer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HashContainer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "HashContainer - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/HashContainer
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\HashCont.ppl"


CLEAN :
	-@erase "$(INTDIR)\hashcontainer.obj"
	-@erase "$(INTDIR)\hashcontainer.res"
	-@erase "$(INTDIR)\plugin_hashcont.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HashCont.exp"
	-@erase "$(OUTDIR)\HashCont.pdb"
	-@erase "$(OUTDIR)\HashCont.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\Include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HASHCONTAINER_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\hashcontainer.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HashContainer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x63e00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\HashCont.pdb" /debug /machine:I386 /out:"$(OUTDIR)\HashCont.ppl" /implib:"$(OUTDIR)\HashCont.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\hashcontainer.obj" \
	"$(INTDIR)\plugin_hashcont.obj" \
	"$(INTDIR)\hashcontainer.res"

"$(OUTDIR)\HashCont.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\HashCont.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\HashCont.ppl"
   call prconvert "\out\Release\HashCont.ppl"
	tsigner "\out\Release\HashCont.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "HashContainer - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/HashContainer
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\HashCont.ppl"


CLEAN :
	-@erase "$(INTDIR)\hashcontainer.obj"
	-@erase "$(INTDIR)\hashcontainer.res"
	-@erase "$(INTDIR)\plugin_hashcont.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HashCont.exp"
	-@erase "$(OUTDIR)\HashCont.ilk"
	-@erase "$(OUTDIR)\HashCont.pdb"
	-@erase "$(OUTDIR)\HashCont.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\Include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HASHCONTAINER_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\hashcontainer.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HashContainer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\HashCont.pdb" /debug /machine:I386 /out:"$(OUTDIR)\HashCont.ppl" /implib:"$(OUTDIR)\HashCont.lib" /pdbtype:sept /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\hashcontainer.obj" \
	"$(INTDIR)\plugin_hashcont.obj" \
	"$(INTDIR)\hashcontainer.res"

"$(OUTDIR)\HashCont.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("HashContainer.dep")
!INCLUDE "HashContainer.dep"
!ELSE 
!MESSAGE Warning: cannot find "HashContainer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HashContainer - Win32 Release" || "$(CFG)" == "HashContainer - Win32 Debug"
SOURCE=.\hashcontainer.c

"$(INTDIR)\hashcontainer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_hashcont.c

"$(INTDIR)\plugin_hashcont.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hashcontainer.rc

"$(INTDIR)\hashcontainer.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


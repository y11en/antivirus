# Microsoft Developer Studio Generated NMAKE File, Based on Avp_iont.dsp
!IF "$(CFG)" == ""
CFG=Avp_ioNT - Win32 Release
!MESSAGE No configuration specified. Defaulting to Avp_ioNT - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Avp_ioNT - Win32 Release" && "$(CFG)" != "Avp_ioNT - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp_iont.mak" CFG="Avp_ioNT - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp_ioNT - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Avp_ioNT - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Avp_ioNT - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\Avp_iont.dll"


CLEAN :
	-@erase "$(INTDIR)\Avp_ioNT.obj"
	-@erase "$(INTDIR)\Avp_ioNT.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp_iont.dll"
	-@erase "$(OUTDIR)\Avp_iont.exp"
	-@erase "$(OUTDIR)\Avp_iont.lib"
	-@erase "$(OUTDIR)\Avp_iont.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Avp_ioNT.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp_iont.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib msvcrt.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Avp_iont.pdb" /debug /machine:I386 /nodefaultlib /def:".\Avp_ioNT.def" /out:"$(OUTDIR)\Avp_iont.dll" /implib:"$(OUTDIR)\Avp_iont.lib" /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\Avp_ioNT.obj" \
	"$(INTDIR)\Avp_ioNT.res"

"$(OUTDIR)\Avp_iont.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\release\Avp_iont.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Avp_iont.dll"
   tsigner  \out\release\Avp_iont.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Avp_ioNT - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug

ALL : "..\..\out\debug\Avp_iont.dll"


CLEAN :
	-@erase "$(INTDIR)\Avp_ioNT.obj"
	-@erase "$(INTDIR)\Avp_ioNT.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp_iont.exp"
	-@erase "$(OUTDIR)\Avp_iont.lib"
	-@erase "$(OUTDIR)\Avp_iont.pdb"
	-@erase "..\..\out\debug\Avp_iont.dll"
	-@erase "..\..\out\debug\Avp_iont.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Avp_ioNT.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp_iont.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\Avp_iont.pdb" /debug /machine:I386 /def:".\Avp_ioNT.def" /out:"../../out/debug/Avp_iont.dll" /implib:"$(OUTDIR)\Avp_iont.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Avp_ioNT.obj" \
	"$(INTDIR)\Avp_ioNT.res"

"..\..\out\debug\Avp_iont.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\debug\Avp_iont.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\out\debug\Avp_iont.dll"
   tsigner  \out\debug\Avp_iont.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

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
!IF EXISTS("Avp_iont.dep")
!INCLUDE "Avp_iont.dep"
!ELSE 
!MESSAGE Warning: cannot find "Avp_iont.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Avp_ioNT - Win32 Release" || "$(CFG)" == "Avp_ioNT - Win32 Debug"
SOURCE=.\Avp_ioNT.c

"$(INTDIR)\Avp_ioNT.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ntllio.c
SOURCE=.\Avp_ioNT.rc

"$(INTDIR)\Avp_ioNT.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on Avp_io32.dsp
!IF "$(CFG)" == ""
CFG=Avp_io32 - Win32 Release
!MESSAGE No configuration specified. Defaulting to Avp_io32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Avp_io32 - Win32 Release" && "$(CFG)" != "Avp_io32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp_io32.mak" CFG="Avp_io32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp_io32 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Avp_io32 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Avp_io32 - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Avp_io32.dll"

!ELSE 

ALL : "Avp_io VxD - Win32 Release" "$(OUTDIR)\Avp_io32.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Avp_io VxD - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Avp_io32.obj"
	-@erase "$(INTDIR)\Avp_io32.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp_io32.dll"
	-@erase "$(OUTDIR)\Avp_io32.exp"
	-@erase "$(OUTDIR)\Avp_io32.lib"
	-@erase "$(OUTDIR)\Avp_io32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /I "..\..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN95" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Avp_io32.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp_io32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib msvcrt.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\Avp_io32.pdb" /debug /machine:I386 /nodefaultlib /def:".\avp_io32.def" /out:"$(OUTDIR)\Avp_io32.dll" /implib:"$(OUTDIR)\Avp_io32.lib" /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\Avp_io32.obj" \
	"$(INTDIR)\Avp_io32.res"

"$(OUTDIR)\Avp_io32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\release\Avp_io32.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "Avp_io VxD - Win32 Release" "$(OUTDIR)\Avp_io32.dll"
   tsigner  \out\release\Avp_io32.dll
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Avp_io32 - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\out\debug\Avp_io32.dll"

!ELSE 

ALL : "Avp_io VxD - Win32 Debug" "..\..\out\debug\Avp_io32.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Avp_io VxD - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\Avp_io32.obj"
	-@erase "$(INTDIR)\Avp_io32.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp_io32.exp"
	-@erase "$(OUTDIR)\Avp_io32.lib"
	-@erase "..\..\out\debug\Avp_io32.dll"
	-@erase "..\..\out\debug\Avp_io32.ilk"
	-@erase ".\Debug\Avp_io32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN95" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Avp_io32.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp_io32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib libc.lib user32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:yes /pdb:".\Debug\Avp_io32.pdb" /debug /machine:I386 /def:".\avp_io32.def" /out:"../../out/debug/Avp_io32.dll" /implib:"$(OUTDIR)\Avp_io32.lib" 
LINK32_OBJS= \
	"$(INTDIR)\Avp_io32.obj" \
	"$(INTDIR)\Avp_io32.res"

"..\..\out\debug\Avp_io32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\debug\Avp_io32.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "Avp_io VxD - Win32 Debug" "..\..\out\debug\Avp_io32.dll"
   tsigner  \out\debug\Avp_io32.dll
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
!IF EXISTS("Avp_io32.dep")
!INCLUDE "Avp_io32.dep"
!ELSE 
!MESSAGE Warning: cannot find "Avp_io32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Avp_io32 - Win32 Release" || "$(CFG)" == "Avp_io32 - Win32 Debug"
SOURCE=.\Avp_io32.c

"$(INTDIR)\Avp_io32.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Avp_io32.rc

"$(INTDIR)\Avp_io32.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Avp_io32 - Win32 Release"

"Avp_io VxD - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Release" 
   cd "."

"Avp_io VxD - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "Avp_io32 - Win32 Debug"

"Avp_io VxD - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Debug" 
   cd "."

"Avp_io VxD - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\Avp_io VxD.mak" CFG="Avp_io VxD - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 


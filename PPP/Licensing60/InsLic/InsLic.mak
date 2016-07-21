# Microsoft Developer Studio Generated NMAKE File, Based on InsLic.dsp
!IF "$(CFG)" == ""
CFG=InsLic - Win32 Debug
!MESSAGE No configuration specified. Defaulting to InsLic - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "InsLic - Win32 Release" && "$(CFG)" != "InsLic - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "InsLic.mak" CFG="InsLic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "InsLic - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "InsLic - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "InsLic - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/ppp/licence/InsLic
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\InsLic.dll"


CLEAN :
	-@erase "$(INTDIR)\DllMain.obj"
	-@erase "$(INTDIR)\lic.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\InsLic.dll"
	-@erase "$(OUTDIR)\InsLic.exp"
	-@erase "$(OUTDIR)\InsLic.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "../../../Prague" /I "../../../Prague/Include" /I "../../Include" /I "../../../CommonFiles" /I "../../../CommonFiles/Licensing/Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INSLIC_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\InsLic.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=licpolicy.lib property.lib kldtser.lib swm.lib sign.lib win32utils.lib winavpio.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\InsLic.pdb" /machine:I386 /out:"$(OUTDIR)\InsLic.dll" /implib:"$(OUTDIR)\InsLic.lib" /libpath:"../../../out/release" 
LINK32_OBJS= \
	"$(INTDIR)\DllMain.obj" \
	"$(INTDIR)\lic.obj"

"$(OUTDIR)\InsLic.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "InsLic - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/ppp/licence/InsLic
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\InsLic.dll"


CLEAN :
	-@erase "$(INTDIR)\DllMain.obj"
	-@erase "$(INTDIR)\lic.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\InsLic.dll"
	-@erase "$(OUTDIR)\InsLic.exp"
	-@erase "$(OUTDIR)\InsLic.ilk"
	-@erase "$(OUTDIR)\InsLic.lib"
	-@erase "$(OUTDIR)\InsLic.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../Prague" /I "../../../Prague/Include" /I "../../Include" /I "../../../CommonFiles" /I "../../../CommonFiles/Licensing/Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INSLIC_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\InsLic.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=licpolicy.lib property.lib kldtser.lib swm.lib sign.lib win32utils.lib winavpio.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\InsLic.pdb" /debug /machine:I386 /out:"$(OUTDIR)\InsLic.dll" /implib:"$(OUTDIR)\InsLic.lib" /pdbtype:sept /libpath:"../../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\DllMain.obj" \
	"$(INTDIR)\lic.obj"

"$(OUTDIR)\InsLic.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("InsLic.dep")
!INCLUDE "InsLic.dep"
!ELSE 
!MESSAGE Warning: cannot find "InsLic.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "InsLic - Win32 Release" || "$(CFG)" == "InsLic - Win32 Debug"
SOURCE=.\DllMain.cpp

"$(INTDIR)\DllMain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lic.cpp

"$(INTDIR)\lic.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


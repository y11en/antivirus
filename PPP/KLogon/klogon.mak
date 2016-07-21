# Microsoft Developer Studio Generated NMAKE File, Based on klogon.dsp
!IF "$(CFG)" == ""
CFG=klogon - Win32 Debug
!MESSAGE No configuration specified. Defaulting to klogon - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "klogon - Win32 Release" && "$(CFG)" != "klogon - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klogon.mak" CFG="klogon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klogon - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klogon - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "klogon - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/klogon
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\klogon.dll"


CLEAN :
	-@erase "$(INTDIR)\klogon.obj"
	-@erase "$(INTDIR)\klogon.res"
	-@erase "$(INTDIR)\Picture.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\klogon.dll"
	-@erase "$(OUTDIR)\klogon.exp"
	-@erase "$(OUTDIR)\klogon.lib"
	-@erase "$(OUTDIR)\klogon.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../Prague" /I "../../Prague/Include" /I "../Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "klogon_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\klogon.res" /i "..\..\..\CommonFiles" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\klogon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\klogon.pdb" /debug /machine:I386 /def:".\klogon.def" /out:"$(OUTDIR)\klogon.dll" /implib:"$(OUTDIR)\klogon.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\klogon.obj" \
	"$(INTDIR)\Picture.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\klogon.res"

"$(OUTDIR)\klogon.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\klogon.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\klogon.dll"
   tsigner "\out\Release\klogon.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "klogon - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/klogon
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\klogon.dll"


CLEAN :
	-@erase "$(INTDIR)\klogon.obj"
	-@erase "$(INTDIR)\klogon.res"
	-@erase "$(INTDIR)\Picture.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\klogon.dll"
	-@erase "$(OUTDIR)\klogon.exp"
	-@erase "$(OUTDIR)\klogon.ilk"
	-@erase "$(OUTDIR)\klogon.lib"
	-@erase "$(OUTDIR)\klogon.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../Prague" /I "../../Prague/Include" /I "../Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "klogon_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\klogon.res" /i "..\..\..\CommonFiles" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\klogon.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\klogon.pdb" /debug /machine:I386 /def:".\klogon.def" /out:"$(OUTDIR)\klogon.dll" /implib:"$(OUTDIR)\klogon.lib" /pdbtype:sept 
DEF_FILE= \
	".\klogon.def"
LINK32_OBJS= \
	"$(INTDIR)\klogon.obj" \
	"$(INTDIR)\Picture.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\klogon.res"

"$(OUTDIR)\klogon.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("klogon.dep")
!INCLUDE "klogon.dep"
!ELSE 
!MESSAGE Warning: cannot find "klogon.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "klogon - Win32 Release" || "$(CFG)" == "klogon - Win32 Debug"
SOURCE=.\klogon.cpp

"$(INTDIR)\klogon.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\BaseGUI\WinGUI\Picture.cpp

"$(INTDIR)\Picture.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\CommonFiles\Service\sa.cpp

"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\klogon.rc

"$(INTDIR)\klogon.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


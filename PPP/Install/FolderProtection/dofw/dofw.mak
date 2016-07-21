# Microsoft Developer Studio Generated NMAKE File, Based on dofw.dsp
!IF "$(CFG)" == ""
CFG=dofw - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dofw - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dofw - Win32 Release" && "$(CFG)" != "dofw - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dofw.mak" CFG="dofw - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dofw - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dofw - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dofw - Win32 Release"

OUTDIR=.\../../../../out/Release
INTDIR=.\../../../../temp/Release/ppp/dofw
# Begin Custom Macros
OutDir=.\../../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\dofw.dll" "..\..\..\..\temp\Release\ppp\dofw\dofw.pch"


CLEAN :
	-@erase "$(INTDIR)\dofw.obj"
	-@erase "$(INTDIR)\dofw.pch"
	-@erase "$(INTDIR)\dofw.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dofw.dll"
	-@erase "$(OUTDIR)\dofw.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G6 /MD /W3 /GX /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "DOFW_EXPORTS" /D "UNICODE" /D "_UNICODE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\dofw.res" /i "..\..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dofw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fsdrvlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x63500000" /dll /incremental:no /pdb:"$(OUTDIR)\dofw.pdb" /machine:I386 /def:".\dofw.def" /out:"$(OUTDIR)\dofw.dll" /implib:"$(OUTDIR)\dofw.lib" /libpath:"../../../../Out/Release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\dofw.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\dofw.res"

"$(OUTDIR)\dofw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\dofw.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\dofw.dll" "..\..\..\..\temp\Release\ppp\dofw\dofw.pch"
   tsigner "\out\Release\dofw.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dofw - Win32 Debug"

OUTDIR=.\../../../../out/Debug
INTDIR=.\../../../../temp/Debug/ppp/dofw
# Begin Custom Macros
OutDir=.\../../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\dofw.dll" "..\..\..\..\temp\Debug\ppp\dofw\dofw.pch"


CLEAN :
	-@erase "$(INTDIR)\dofw.obj"
	-@erase "$(INTDIR)\dofw.pch"
	-@erase "$(INTDIR)\dofw.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dofw.dll"
	-@erase "$(OUTDIR)\dofw.exp"
	-@erase "$(OUTDIR)\dofw.ilk"
	-@erase "$(OUTDIR)\dofw.lib"
	-@erase "$(OUTDIR)\dofw.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "DOFW_EXPORTS" /D "UNICODE" /D "_UNICODE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\dofw.res" /i "..\..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dofw.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=fsdrvlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\dofw.pdb" /debug /machine:I386 /def:".\dofw.def" /out:"$(OUTDIR)\dofw.dll" /implib:"$(OUTDIR)\dofw.lib" /pdbtype:sept /libpath:"../../../../Out/Debug" 
DEF_FILE= \
	".\dofw.def"
LINK32_OBJS= \
	"$(INTDIR)\dofw.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\dofw.res"

"$(OUTDIR)\dofw.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dofw.dep")
!INCLUDE "dofw.dep"
!ELSE 
!MESSAGE Warning: cannot find "dofw.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dofw - Win32 Release" || "$(CFG)" == "dofw - Win32 Debug"
SOURCE=.\dofw.cpp

"$(INTDIR)\dofw.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dofw.rc

"$(INTDIR)\dofw.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "dofw - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /GX /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "DOFW_EXPORTS" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)\dofw.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\dofw.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "dofw - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "DOFW_EXPORTS" /D "UNICODE" /D "_UNICODE" /Fp"$(INTDIR)\dofw.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\dofw.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on btdisk.dsp
!IF "$(CFG)" == ""
CFG=btdisk - Win32 Debug
!MESSAGE No configuration specified. Defaulting to btdisk - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "btdisk - Win32 Release" && "$(CFG)" != "btdisk - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "btdisk.mak" CFG="btdisk - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "btdisk - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "btdisk - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "btdisk - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/btdisk
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\btdisk.ppl"


CLEAN :
	-@erase "$(INTDIR)\btdiskimp.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\plugin_btdisk.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\btdisk.exp"
	-@erase "$(OUTDIR)\btdisk.pdb"
	-@erase "$(OUTDIR)\btdisk.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "btdisk_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\btdisk.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib /nologo /base:"0x62900000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\btdisk.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\btdisk.ppl" /implib:"$(OUTDIR)\btdisk.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDLL" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\btdiskimp.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\plugin_btdisk.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\btdisk.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\btdisk.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\btdisk.ppl"
   call prconvert "\out\Release\btdisk.ppl"
	tsigner "\out\Release\btdisk.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "btdisk - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/btdisk
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\btdisk.ppl"


CLEAN :
	-@erase "$(INTDIR)\btdiskimp.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\plugin_btdisk.obj"
	-@erase "$(INTDIR)\template.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\btdisk.exp"
	-@erase "$(OUTDIR)\btdisk.ilk"
	-@erase "$(OUTDIR)\btdisk.pdb"
	-@erase "$(OUTDIR)\btdisk.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "." /I ".." /I "..\include" /I "..\include\iface" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "btdisk_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\template.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\btdisk.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /dll /incremental:yes /pdb:"$(OUTDIR)\btdisk.pdb" /debug /machine:I386 /out:"$(OUTDIR)\btdisk.ppl" /implib:"$(OUTDIR)\btdisk.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDLL" 
LINK32_OBJS= \
	"$(INTDIR)\btdiskimp.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\plugin_btdisk.obj" \
	"$(INTDIR)\template.res"

"$(OUTDIR)\btdisk.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("btdisk.dep")
!INCLUDE "btdisk.dep"
!ELSE 
!MESSAGE Warning: cannot find "btdisk.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "btdisk - Win32 Release" || "$(CFG)" == "btdisk - Win32 Debug"
SOURCE=.\btdiskimp.cpp

"$(INTDIR)\btdiskimp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\io.cpp

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objptr.cpp

"$(INTDIR)\objptr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os.cpp

"$(INTDIR)\os.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_btdisk.cpp

"$(INTDIR)\plugin_btdisk.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\template.rc

"$(INTDIR)\template.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


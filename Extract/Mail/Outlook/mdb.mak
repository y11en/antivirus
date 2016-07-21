# Microsoft Developer Studio Generated NMAKE File, Based on mdb.dsp
!IF "$(CFG)" == ""
CFG=mdb - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mdb - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mdb - Win32 Release" && "$(CFG)" != "mdb - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdb.mak" CFG="mdb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mdb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mdb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "mdb - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/Outlook
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\mdb.ppl"


CLEAN :
	-@erase "$(INTDIR)\cmn_mapi.obj"
	-@erase "$(INTDIR)\hook.obj"
	-@erase "$(INTDIR)\mdb.res"
	-@erase "$(INTDIR)\mdb_io.obj"
	-@erase "$(INTDIR)\mdb_mapi.obj"
	-@erase "$(INTDIR)\mdb_objptr.obj"
	-@erase "$(INTDIR)\mdb_os.obj"
	-@erase "$(INTDIR)\plugin_mdb.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mdb.exp"
	-@erase "$(OUTDIR)\mdb.pdb"
	-@erase "$(OUTDIR)\mdb.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "..\iface\include" /I "..\include\iface" /I "..\include" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "mdb_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\mdb.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mdb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib mapi32.lib advapi32.lib version.lib user32.lib /nologo /base:"0x65300000" /dll /incremental:no /pdb:"$(OUTDIR)\mdb.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mdb.ppl" /implib:"$(OUTDIR)\mdb.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\cmn_mapi.obj" \
	"$(INTDIR)\hook.obj" \
	"$(INTDIR)\mdb_io.obj" \
	"$(INTDIR)\mdb_mapi.obj" \
	"$(INTDIR)\mdb_objptr.obj" \
	"$(INTDIR)\mdb_os.obj" \
	"$(INTDIR)\plugin_mdb.obj" \
	"$(INTDIR)\mdb.res"

"$(OUTDIR)\mdb.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\mdb.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\mdb.ppl"
   tsigner "\out\Release\mdb.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/Outlook
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\mdb.ppl" "$(OUTDIR)\mdb.bsc"


CLEAN :
	-@erase "$(INTDIR)\cmn_mapi.obj"
	-@erase "$(INTDIR)\cmn_mapi.sbr"
	-@erase "$(INTDIR)\hook.obj"
	-@erase "$(INTDIR)\hook.sbr"
	-@erase "$(INTDIR)\mdb.res"
	-@erase "$(INTDIR)\mdb_io.obj"
	-@erase "$(INTDIR)\mdb_io.sbr"
	-@erase "$(INTDIR)\mdb_mapi.obj"
	-@erase "$(INTDIR)\mdb_mapi.sbr"
	-@erase "$(INTDIR)\mdb_objptr.obj"
	-@erase "$(INTDIR)\mdb_objptr.sbr"
	-@erase "$(INTDIR)\mdb_os.obj"
	-@erase "$(INTDIR)\mdb_os.sbr"
	-@erase "$(INTDIR)\plugin_mdb.obj"
	-@erase "$(INTDIR)\plugin_mdb.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mdb.bsc"
	-@erase "$(OUTDIR)\mdb.exp"
	-@erase "$(OUTDIR)\mdb.ilk"
	-@erase "$(OUTDIR)\mdb.map"
	-@erase "$(OUTDIR)\mdb.pdb"
	-@erase "$(OUTDIR)\mdb.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "..\include\iface" /I "..\include" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "mdb_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\mdb.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mdb.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cmn_mapi.sbr" \
	"$(INTDIR)\hook.sbr" \
	"$(INTDIR)\mdb_io.sbr" \
	"$(INTDIR)\mdb_mapi.sbr" \
	"$(INTDIR)\mdb_objptr.sbr" \
	"$(INTDIR)\mdb_os.sbr" \
	"$(INTDIR)\plugin_mdb.sbr"

"$(OUTDIR)\mdb.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib mapi32.lib advapi32.lib version.lib user32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\mdb.pdb" /map:"../../out/Debug/mdb.map" /debug /machine:I386 /out:"$(OUTDIR)\mdb.ppl" /implib:"$(OUTDIR)\mdb.lib" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\cmn_mapi.obj" \
	"$(INTDIR)\hook.obj" \
	"$(INTDIR)\mdb_io.obj" \
	"$(INTDIR)\mdb_mapi.obj" \
	"$(INTDIR)\mdb_objptr.obj" \
	"$(INTDIR)\mdb_os.obj" \
	"$(INTDIR)\plugin_mdb.obj" \
	"$(INTDIR)\mdb.res"

"$(OUTDIR)\mdb.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("mdb.dep")
!INCLUDE "mdb.dep"
!ELSE 
!MESSAGE Warning: cannot find "mdb.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mdb - Win32 Release" || "$(CFG)" == "mdb - Win32 Debug"
SOURCE=.\cmn_mapi.cpp

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\cmn_mapi.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\cmn_mapi.obj"	"$(INTDIR)\cmn_mapi.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\hook.cpp

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\hook.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\hook.obj"	"$(INTDIR)\hook.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mdb_io.c

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\mdb_io.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\mdb_io.obj"	"$(INTDIR)\mdb_io.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mdb_mapi.cpp

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\mdb_mapi.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\mdb_mapi.obj"	"$(INTDIR)\mdb_mapi.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mdb_objptr.c

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\mdb_objptr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\mdb_objptr.obj"	"$(INTDIR)\mdb_objptr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mdb_os.c

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\mdb_os.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\mdb_os.obj"	"$(INTDIR)\mdb_os.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_mdb.c

!IF  "$(CFG)" == "mdb - Win32 Release"


"$(INTDIR)\plugin_mdb.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mdb - Win32 Debug"


"$(INTDIR)\plugin_mdb.obj"	"$(INTDIR)\plugin_mdb.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mdb.rc

"$(INTDIR)\mdb.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


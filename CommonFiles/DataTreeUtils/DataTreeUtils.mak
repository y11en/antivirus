# Microsoft Developer Studio Generated NMAKE File, Based on DataTreeUtils.dsp
!IF "$(CFG)" == ""
CFG=DataTreeUtils - Win32 Unicode Debug
!MESSAGE No configuration specified. Defaulting to DataTreeUtils - Win32 Unicode Debug.
!ENDIF 

!IF "$(CFG)" != "DataTreeUtils - Win32 Release" && "$(CFG)" != "DataTreeUtils - Win32 Debug" && "$(CFG)" != "DataTreeUtils - Win32 DebugDll" && "$(CFG)" != "DataTreeUtils - Win32 ReleaseDll" && "$(CFG)" != "DataTreeUtils - Win32 Unicode DebugDll" && "$(CFG)" != "DataTreeUtils - Win32 Unicode ReleaseDll" && "$(CFG)" != "DataTreeUtils - Win32 Unicode Release" && "$(CFG)" != "DataTreeUtils - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DataTreeUtils.mak" CFG="DataTreeUtils - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DataTreeUtils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DataTreeUtils - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DataTreeUtils - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\CommonFiles\Release\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\Release\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G4 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Release\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\Release\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\CommonFiles\Debug\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\Debug\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Debug\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\Debug\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 DebugDll"

OUTDIR=.\DebugDll
INTDIR=.\DebugDll

ALL : "..\CommonFiles\DebugDll\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\DebugDll\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugDll\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\DebugDll\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 ReleaseDll"

OUTDIR=.\ReleaseDll
INTDIR=.\ReleaseDll

ALL : "..\CommonFiles\ReleaseDll\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseDll\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G4 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseDll\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\ReleaseDll\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode DebugDll"

OUTDIR=.\DebugUDll
INTDIR=.\DebugUDll

ALL : "..\CommonFiles\DebugUDll\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\DebugUDll\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G4 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugUDll\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\DebugUDll\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode ReleaseDll"

OUTDIR=.\ReleaseUDll
INTDIR=.\ReleaseUDll

ALL : "..\CommonFiles\ReleaseUDll\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseUDll\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G4 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseUDll\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\ReleaseUDll\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode Release"

OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : "..\CommonFiles\ReleaseU\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseU\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G4 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseU\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\ReleaseU\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DataTreeUtils - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU

ALL : "..\CommonFiles\DebugU\DTUtils.lib"


CLEAN :
	-@erase "$(INTDIR)\CmpObjs.obj"
	-@erase "$(INTDIR)\CmpObjsW.obj"
	-@erase "$(INTDIR)\CompTree.obj"
	-@erase "$(INTDIR)\DtPtrnME.obj"
	-@erase "$(INTDIR)\DTUInit.obj"
	-@erase "$(INTDIR)\ExchProp.obj"
	-@erase "$(INTDIR)\FindGet.obj"
	-@erase "$(INTDIR)\PIDFind.obj"
	-@erase "$(INTDIR)\PrSetGet.obj"
	-@erase "$(INTDIR)\PrSetGetW.obj"
	-@erase "$(INTDIR)\RuleConv.obj"
	-@erase "$(INTDIR)\SearRule.obj"
	-@erase "$(INTDIR)\SearRuleW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\DebugU\DTUtils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G4 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DataTreeUtils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugU\DTUtils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CmpObjs.obj" \
	"$(INTDIR)\CmpObjsW.obj" \
	"$(INTDIR)\CompTree.obj" \
	"$(INTDIR)\DtPtrnME.obj" \
	"$(INTDIR)\DTUInit.obj" \
	"$(INTDIR)\ExchProp.obj" \
	"$(INTDIR)\FindGet.obj" \
	"$(INTDIR)\PIDFind.obj" \
	"$(INTDIR)\PrSetGet.obj" \
	"$(INTDIR)\PrSetGetW.obj" \
	"$(INTDIR)\RuleConv.obj" \
	"$(INTDIR)\SearRule.obj" \
	"$(INTDIR)\SearRuleW.obj"

"..\CommonFiles\DebugU\DTUtils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("DataTreeUtils.dep")
!INCLUDE "DataTreeUtils.dep"
!ELSE 
!MESSAGE Warning: cannot find "DataTreeUtils.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DataTreeUtils - Win32 Release" || "$(CFG)" == "DataTreeUtils - Win32 Debug" || "$(CFG)" == "DataTreeUtils - Win32 DebugDll" || "$(CFG)" == "DataTreeUtils - Win32 ReleaseDll" || "$(CFG)" == "DataTreeUtils - Win32 Unicode DebugDll" || "$(CFG)" == "DataTreeUtils - Win32 Unicode ReleaseDll" || "$(CFG)" == "DataTreeUtils - Win32 Unicode Release" || "$(CFG)" == "DataTreeUtils - Win32 Unicode Debug"
SOURCE=.\CmpObjs.c

"$(INTDIR)\CmpObjs.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CmpObjsW.c

"$(INTDIR)\CmpObjsW.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CompTree.c

"$(INTDIR)\CompTree.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DtPtrnME.c

"$(INTDIR)\DtPtrnME.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DTUInit.c

"$(INTDIR)\DTUInit.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ExchProp.c

"$(INTDIR)\ExchProp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\FindGet.c

"$(INTDIR)\FindGet.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PIDFind.c

"$(INTDIR)\PIDFind.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PrSetGet.c

"$(INTDIR)\PrSetGet.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PrSetGetW.c

"$(INTDIR)\PrSetGetW.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RuleConv.c

"$(INTDIR)\RuleConv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SearRule.c

"$(INTDIR)\SearRule.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SearRuleW.c

"$(INTDIR)\SearRuleW.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


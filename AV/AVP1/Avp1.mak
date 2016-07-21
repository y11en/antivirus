# Microsoft Developer Studio Generated NMAKE File, Based on Avp1.dsp
!IF "$(CFG)" == ""
CFG=Avp1 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Avp1 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Avp1 - Win32 Release" && "$(CFG)" != "Avp1 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp1.mak" CFG="Avp1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp1 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Avp1 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Avp1 - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/AVP1
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Avp1.ppl"

!ELSE 

ALL : "FSDrvLib - Win32 Release" "BaseWork - Win32 Release" "$(OUTDIR)\Avp1.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"BaseWork - Win32 ReleaseCLEAN" "FSDrvLib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dprintf.obj"
	-@erase "$(INTDIR)\DupMem.obj"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.res"
	-@erase "$(INTDIR)\IniLib.obj"
	-@erase "$(INTDIR)\plugin_avp1.obj"
	-@erase "$(INTDIR)\redirs.obj"
	-@erase "$(INTDIR)\SpcAllocPg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp1.exp"
	-@erase "$(OUTDIR)\Avp1.pdb"
	-@erase "$(OUTDIR)\Avp1.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /I "..\..\avp32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\engine.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=basework.lib kldtser.lib property.lib swm.lib FSDrvLib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sign.lib /nologo /base:"0x61c00000" /dll /incremental:no /pdb:"$(OUTDIR)\Avp1.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Avp1.ppl" /implib:"$(OUTDIR)\Avp1.lib" /pdbtype:sept /libpath:"..\..\out\Release" /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\dprintf.obj" \
	"$(INTDIR)\DupMem.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\IniLib.obj" \
	"$(INTDIR)\plugin_avp1.obj" \
	"$(INTDIR)\redirs.obj" \
	"$(INTDIR)\SpcAllocPg.obj" \
	"$(INTDIR)\engine.res" \
	"$(OUTDIR)\BaseWork.lib" \
	"$(OUTDIR)\FSDrvLib.lib"

"$(OUTDIR)\Avp1.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\Avp1.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "FSDrvLib - Win32 Release" "BaseWork - Win32 Release" "$(OUTDIR)\Avp1.ppl"
   tsigner "\out\Release\Avp1.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Avp1 - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/AVP1
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Avp1.ppl"

!ELSE 

ALL : "FSDrvLib - Win32 Debug" "BaseWork - Win32 Debug" "$(OUTDIR)\Avp1.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"BaseWork - Win32 DebugCLEAN" "FSDrvLib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dprintf.obj"
	-@erase "$(INTDIR)\DupMem.obj"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.res"
	-@erase "$(INTDIR)\IniLib.obj"
	-@erase "$(INTDIR)\plugin_avp1.obj"
	-@erase "$(INTDIR)\redirs.obj"
	-@erase "$(INTDIR)\SpcAllocPg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp1.exp"
	-@erase "$(OUTDIR)\Avp1.ilk"
	-@erase "$(OUTDIR)\Avp1.pdb"
	-@erase "$(OUTDIR)\Avp1.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\Include" /I "..\..\avp32" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\engine.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp1.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=basework.lib FSDrvLib.lib kldtser.lib property.lib swm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sign.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Avp1.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Avp1.ppl" /implib:"$(OUTDIR)\Avp1.lib" /pdbtype:sept /libpath:"..\..\out\Debug" /libpath:"..\..\CommonFiles\DebugDll" /IGNORE:4098 /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\dprintf.obj" \
	"$(INTDIR)\DupMem.obj" \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\IniLib.obj" \
	"$(INTDIR)\plugin_avp1.obj" \
	"$(INTDIR)\redirs.obj" \
	"$(INTDIR)\SpcAllocPg.obj" \
	"$(INTDIR)\engine.res" \
	"$(OUTDIR)\BaseWork.lib" \
	"$(OUTDIR)\FSDrvLib.lib"

"$(OUTDIR)\Avp1.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Avp1.dep")
!INCLUDE "Avp1.dep"
!ELSE 
!MESSAGE Warning: cannot find "Avp1.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Avp1 - Win32 Release" || "$(CFG)" == "Avp1 - Win32 Debug"
SOURCE=..\..\CommonFiles\Stuff\dprintf.c

"$(INTDIR)\dprintf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DupMem.cpp

"$(INTDIR)\DupMem.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\engine.c

!IF  "$(CFG)" == "Avp1 - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /I "..\..\avp32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Avp1 - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\Include" /I "..\..\avp32" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\IniLib\IniLib.c

"$(INTDIR)\IniLib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_avp1.c

!IF  "$(CFG)" == "Avp1 - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /I "..\..\avp32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\plugin_avp1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Avp1 - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\Include" /I "..\..\avp32" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\plugin_avp1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\redirs.cpp

!IF  "$(CFG)" == "Avp1 - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\Include" /I "..\..\avp32" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\redirs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Avp1 - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\Include" /I "..\..\avp32" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVP1_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\redirs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SpcAllocPg.cpp

"$(INTDIR)\SpcAllocPg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\engine.rc

"$(INTDIR)\engine.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Avp1 - Win32 Release"

"BaseWork - Win32 Release" : 
   cd "\AVP32\basework"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Release" 
   cd "..\..\prague\avp1"

"BaseWork - Win32 ReleaseCLEAN" : 
   cd "\AVP32\basework"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\prague\avp1"

!ELSEIF  "$(CFG)" == "Avp1 - Win32 Debug"

"BaseWork - Win32 Debug" : 
   cd "\AVP32\basework"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Debug" 
   cd "..\..\prague\avp1"

"BaseWork - Win32 DebugCLEAN" : 
   cd "\AVP32\basework"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\prague\avp1"

!ENDIF 

!IF  "$(CFG)" == "Avp1 - Win32 Release"

"FSDrvLib - Win32 Release" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Release" 
   cd "..\..\..\prague\avp1"

"FSDrvLib - Win32 ReleaseCLEAN" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\prague\avp1"

!ELSEIF  "$(CFG)" == "Avp1 - Win32 Debug"

"FSDrvLib - Win32 Debug" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Debug" 
   cd "..\..\..\prague\avp1"

"FSDrvLib - Win32 DebugCLEAN" : 
   cd "\Windows\Hook\FSDrvLib"
   $(MAKE) /$(MAKEFLAGS) /F .\FSDrvLib.mak CFG="FSDrvLib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\prague\avp1"

!ENDIF 


!ENDIF 


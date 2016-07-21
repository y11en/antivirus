# Microsoft Developer Studio Generated NMAKE File, Based on AvpBase.dsp
!IF "$(CFG)" == ""
CFG=AvpBase - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AvpBase - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AvpBase - Win32 Release" && "$(CFG)" != "AvpBase - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvpBase.mak" CFG="AvpBase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvpBase - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AvpBase - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AvpBase - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AvpBase.dll"

!ELSE 

ALL : "BaseWork - Win32 Release" "Avp_ioNT - Win32 Release" "Avp_io32 - Win32 Release" "$(OUTDIR)\AvpBase.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Avp_io32 - Win32 ReleaseCLEAN" "Avp_ioNT - Win32 ReleaseCLEAN" "BaseWork - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AvpBase.obj"
	-@erase "$(INTDIR)\AvpBase.pch"
	-@erase "$(INTDIR)\AvpBase.res"
	-@erase "$(INTDIR)\FileIO.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AvpBase.dll"
	-@erase "$(OUTDIR)\AvpBase.exp"
	-@erase "$(OUTDIR)\AvpBase.lib"
	-@erase "$(OUTDIR)\AvpBase.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /I "..\\" /I "..\..\commonfiles" /D "MULTITHREAD" /D "USE_WIN32_API" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPBASE_EXPORTS" /Fp"$(INTDIR)\AvpBase.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\AvpBase.res" /i "..\..\commonfiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AvpBase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kldtser.lib property.lib swm.lib sign.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AvpBase.pdb" /debug /machine:I386 /def:".\Avpbase.def" /out:"$(OUTDIR)\AvpBase.dll" /implib:"$(OUTDIR)\AvpBase.lib" /libpath:"../../out/Release" /MAPINFO:LINES /ALIGN:4096 /IGNORE:4108 /fixed:no 
LINK32_OBJS= \
	"$(INTDIR)\AvpBase.obj" \
	"$(INTDIR)\FileIO.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AvpBase.res" \
	"..\Release\Avp_io32.lib" \
	"..\Release\Avp_iont.lib" \
	"$(OUTDIR)\BaseWork.lib"

"$(OUTDIR)\AvpBase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\release\AvpBase.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "BaseWork - Win32 Release" "Avp_ioNT - Win32 Release" "Avp_io32 - Win32 Release" "$(OUTDIR)\AvpBase.dll"
   tsigner "\out\release\AvpBase.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "AvpBase - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AvpBase.dll"

!ELSE 

ALL : "BaseWork - Win32 Debug" "Avp_ioNT - Win32 Debug" "Avp_io32 - Win32 Debug" "$(OUTDIR)\AvpBase.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Avp_io32 - Win32 DebugCLEAN" "Avp_ioNT - Win32 DebugCLEAN" "BaseWork - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AvpBase.obj"
	-@erase "$(INTDIR)\AvpBase.pch"
	-@erase "$(INTDIR)\AvpBase.res"
	-@erase "$(INTDIR)\FileIO.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AvpBase.dll"
	-@erase "$(OUTDIR)\AvpBase.exp"
	-@erase "$(OUTDIR)\AvpBase.ilk"
	-@erase "$(OUTDIR)\AvpBase.lib"
	-@erase "$(OUTDIR)\AvpBase.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\commonfiles" /D "MULTITHREAD" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPBASE_EXPORTS" /Fp"$(INTDIR)\AvpBase.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\AvpBase.res" /i "..\..\commonfiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AvpBase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kldtser.lib property.lib swm.lib sign.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\AvpBase.pdb" /debug /machine:I386 /def:".\Avpbase.def" /out:"$(OUTDIR)\AvpBase.dll" /implib:"$(OUTDIR)\AvpBase.lib" /libpath:"../../out/debug" -IGNORE:4099 
LINK32_OBJS= \
	"$(INTDIR)\AvpBase.obj" \
	"$(INTDIR)\FileIO.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AvpBase.res" \
	"..\Debug\Avp_io32.lib" \
	"..\Debug\Avp_iont.lib" \
	"$(OUTDIR)\BaseWork.lib"

"$(OUTDIR)\AvpBase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\debug\AvpBase.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "BaseWork - Win32 Debug" "Avp_ioNT - Win32 Debug" "Avp_io32 - Win32 Debug" "$(OUTDIR)\AvpBase.dll"
   tsigner "\out\debug\AvpBase.dll"
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
!IF EXISTS("AvpBase.dep")
!INCLUDE "AvpBase.dep"
!ELSE 
!MESSAGE Warning: cannot find "AvpBase.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AvpBase - Win32 Release" || "$(CFG)" == "AvpBase - Win32 Debug"
SOURCE=.\AvpBase.cpp

"$(INTDIR)\AvpBase.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AvpBase.pch"


SOURCE=.\AvpBase.rc

"$(INTDIR)\AvpBase.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\IOCache\FileIO.cpp

"$(INTDIR)\FileIO.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\AvpBase.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "AvpBase - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /I "..\\" /I "..\..\commonfiles" /D "MULTITHREAD" /D "USE_WIN32_API" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPBASE_EXPORTS" /Fp"$(INTDIR)\AvpBase.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AvpBase.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AvpBase - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\..\commonfiles" /D "MULTITHREAD" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPBASE_EXPORTS" /Fp"$(INTDIR)\AvpBase.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\AvpBase.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

!IF  "$(CFG)" == "AvpBase - Win32 Release"

"Avp_io32 - Win32 Release" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Release" 
   cd "..\AvpBase"

"Avp_io32 - Win32 ReleaseCLEAN" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\AvpBase"

!ELSEIF  "$(CFG)" == "AvpBase - Win32 Debug"

"Avp_io32 - Win32 Debug" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Debug" 
   cd "..\AvpBase"

"Avp_io32 - Win32 DebugCLEAN" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_io32.mak CFG="Avp_io32 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\AvpBase"

!ENDIF 

!IF  "$(CFG)" == "AvpBase - Win32 Release"

"Avp_ioNT - Win32 Release" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Release" 
   cd "..\AvpBase"

"Avp_ioNT - Win32 ReleaseCLEAN" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Release" RECURSE=1 CLEAN 
   cd "..\AvpBase"

!ELSEIF  "$(CFG)" == "AvpBase - Win32 Debug"

"Avp_ioNT - Win32 Debug" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Debug" 
   cd "..\AvpBase"

"Avp_ioNT - Win32 DebugCLEAN" : 
   cd "\avp32\AVP_IO"
   $(MAKE) /$(MAKEFLAGS) /F .\Avp_iont.mak CFG="Avp_ioNT - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\AvpBase"

!ENDIF 

!IF  "$(CFG)" == "AvpBase - Win32 Release"

"BaseWork - Win32 Release" : 
   cd "\avp32\BaseWork"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Release" 
   cd "..\AvpBase"

"BaseWork - Win32 ReleaseCLEAN" : 
   cd "\avp32\BaseWork"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Release" RECURSE=1 CLEAN 
   cd "..\AvpBase"

!ELSEIF  "$(CFG)" == "AvpBase - Win32 Debug"

"BaseWork - Win32 Debug" : 
   cd "\avp32\BaseWork"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Debug" 
   cd "..\AvpBase"

"BaseWork - Win32 DebugCLEAN" : 
   cd "\avp32\BaseWork"
   $(MAKE) /$(MAKEFLAGS) /F .\BaseWork.mak CFG="BaseWork - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\AvpBase"

!ENDIF 


!ENDIF 


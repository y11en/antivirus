# Microsoft Developer Studio Generated NMAKE File, Based on lic60.dsp
!IF "$(CFG)" == ""
CFG=Licensing60 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Licensing60 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Licensing60 - Win32 Release" && "$(CFG)" != "Licensing60 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lic60.mak" CFG="Licensing60 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Licensing60 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Licensing60 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/licence
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\lic60.ppl"

!ELSE 

ALL : "_CommonFiles_NoUnicode - Win32 Release" "licpolicy_no_xml - Win32 Release" "$(OUTDIR)\lic60.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"licpolicy_no_xml - Win32 ReleaseCLEAN" "_CommonFiles_NoUnicode - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\lic60_res.res"
	-@erase "$(INTDIR)\licensing.obj"
	-@erase "$(INTDIR)\plugin_licensing60.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\lic60.exp"
	-@erase "$(OUTDIR)\lic60.pdb"
	-@erase "$(OUTDIR)\lic60.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\lic60_res.res" /i "..\..\..\CommonFiles" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lic60.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=licpolicy_no_xml.lib property.lib kldtser.lib swm.lib sign.lib win32utils.lib winavpio.lib kernel32.lib ole32.lib /nologo /base:"0x64c00000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\lic60.pdb" /debug /machine:I386 /out:"$(OUTDIR)\lic60.ppl" /implib:"$(OUTDIR)\lic60.lib" /pdbtype:sept /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\licensing.obj" \
	"$(INTDIR)\plugin_licensing60.obj" \
	"$(INTDIR)\lic60_res.res" \
	"..\..\CommonFiles\Release\licpolicy_no_xml.lib"

"$(OUTDIR)\lic60.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\lic60.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "_CommonFiles_NoUnicode - Win32 Release" "licpolicy_no_xml - Win32 Release" "$(OUTDIR)\lic60.ppl"
   call prconvert "\out\Release\lic60.ppl"
	tsigner "\out\Release\lic60.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/licence
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\lic60.ppl"

!ELSE 

ALL : "_CommonFiles_NoUnicode - Win32 Debug" "licpolicy_no_xml - Win32 Debug" "$(OUTDIR)\lic60.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"licpolicy_no_xml - Win32 DebugCLEAN" "_CommonFiles_NoUnicode - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\lic60_res.res"
	-@erase "$(INTDIR)\licensing.obj"
	-@erase "$(INTDIR)\plugin_licensing60.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\lic60.exp"
	-@erase "$(OUTDIR)\lic60.ilk"
	-@erase "$(OUTDIR)\lic60.pdb"
	-@erase "$(OUTDIR)\lic60.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\lic60_res.res" /i "..\..\..\CommonFiles" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\lic60.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=licpolicy_no_xml.lib property.lib kldtser.lib swm.lib sign.lib win32utils.lib winavpio.lib kernel32.lib ole32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\lic60.pdb" /debug /machine:I386 /out:"$(OUTDIR)\lic60.ppl" /pdbtype:sept /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\licensing.obj" \
	"$(INTDIR)\plugin_licensing60.obj" \
	"$(INTDIR)\lic60_res.res" \
	"..\..\CommonFiles\Debug\licpolicy_no_xml.lib"

"$(OUTDIR)\lic60.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("lic60.dep")
!INCLUDE "lic60.dep"
!ELSE 
!MESSAGE Warning: cannot find "lic60.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Licensing60 - Win32 Release" || "$(CFG)" == "Licensing60 - Win32 Debug"
SOURCE=.\licensing.cpp

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\licensing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\licensing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\plugin_licensing60.cpp

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "_PRAGUE_TRACE_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\plugin_licensing60.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../Prague" /I "../../Prague/Include" /I "../Include" /I "../../CommonFiles" /I "../../CommonFiles/Licensing/Include" /I "../../windows/OnlineActivation/KeyFileDownloader for 6.0" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\plugin_licensing60.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=".\lic60_res.rc"

"$(INTDIR)\lic60_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Licensing60 - Win32 Release"

"licpolicy_no_xml - Win32 Release" : 
   cd "\CommonFiles\Licensing\build\win32\licpolicy_no_xml"
   $(MAKE) /$(MAKEFLAGS) /F .\licpolicy_no_xml.mak CFG="licpolicy_no_xml - Win32 Release" 
   cd "..\..\..\..\..\PPP\Licensing60"

"licpolicy_no_xml - Win32 ReleaseCLEAN" : 
   cd "\CommonFiles\Licensing\build\win32\licpolicy_no_xml"
   $(MAKE) /$(MAKEFLAGS) /F .\licpolicy_no_xml.mak CFG="licpolicy_no_xml - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\PPP\Licensing60"

!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

"licpolicy_no_xml - Win32 Debug" : 
   cd "\CommonFiles\Licensing\build\win32\licpolicy_no_xml"
   $(MAKE) /$(MAKEFLAGS) /F .\licpolicy_no_xml.mak CFG="licpolicy_no_xml - Win32 Debug" 
   cd "..\..\..\..\..\PPP\Licensing60"

"licpolicy_no_xml - Win32 DebugCLEAN" : 
   cd "\CommonFiles\Licensing\build\win32\licpolicy_no_xml"
   $(MAKE) /$(MAKEFLAGS) /F .\licpolicy_no_xml.mak CFG="licpolicy_no_xml - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\..\..\PPP\Licensing60"

!ENDIF 

!IF  "$(CFG)" == "Licensing60 - Win32 Release"

"_CommonFiles_NoUnicode - Win32 Release" : 
   cd "\CommonFiles"
   NMAKE /f _msdev_make.mak TARGET="All - Win32 ReleaseDll" DSW="CommonFiles"
   cd "..\PPP\Licensing60"

"_CommonFiles_NoUnicode - Win32 ReleaseCLEAN" : 
   cd "\CommonFiles"
   cd "..\PPP\Licensing60"

!ELSEIF  "$(CFG)" == "Licensing60 - Win32 Debug"

"_CommonFiles_NoUnicode - Win32 Debug" : 
   cd "\CommonFiles"
   NMAKE /f _msdev_make.mak TARGET="All - Win32 DebugDll" DSW="CommonFiles"
   cd "..\PPP\Licensing60"

"_CommonFiles_NoUnicode - Win32 DebugCLEAN" : 
   cd "\CommonFiles"
   cd "..\PPP\Licensing60"

!ENDIF 


!ENDIF 


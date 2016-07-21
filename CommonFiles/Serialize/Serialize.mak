# Microsoft Developer Studio Generated NMAKE File, Based on Serialize.dsp
!IF "$(CFG)" == ""
CFG=Serialize - Win32 Unicode Debug
!MESSAGE No configuration specified. Defaulting to Serialize - Win32 Unicode Debug.
!ENDIF 

!IF "$(CFG)" != "Serialize - Win32 Release" && "$(CFG)" != "Serialize - Win32 Debug" && "$(CFG)" != "Serialize - Win32 DebugDll" && "$(CFG)" != "Serialize - Win32 ReleaseDll" && "$(CFG)" != "Serialize - Win32 Unicode DebugDll" && "$(CFG)" != "Serialize - Win32 Unicode Release" && "$(CFG)" != "Serialize - Win32 Unicode Debug" && "$(CFG)" != "Serialize - Win32 Unicode ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Serialize.mak" CFG="Serialize - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Serialize - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Serialize - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "Serialize - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\CommonFiles\Release\KLDTSer.lib"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\Release\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Release\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\Release\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\CommonFiles\Debug\KLDTSer.lib" "$(OUTDIR)\Serialize.bsc"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser.sbr"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\Deser_W.sbr"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserH_W.sbr"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserHdr.sbr"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserMem.sbr"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR.sbr"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserR_W.sbr"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM.sbr"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DeserSWM_W.sbr"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMHdr.sbr"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMR.sbr"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\DesrMSWM.sbr"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst.sbr"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaCst_W.sbr"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaHdr.sbr"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\SeriaInt.sbr"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial.sbr"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\Serial_W.sbr"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW.sbr"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SerialW_W.sbr"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaMem.sbr"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM.sbr"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.sbr"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMCst.sbr"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMHdr.sbr"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMSWM.sbr"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\SeriMW.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Serialize.bsc"
	-@erase "..\CommonFiles\Debug\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Deser.sbr" \
	"$(INTDIR)\Deser_W.sbr" \
	"$(INTDIR)\DeserH_W.sbr" \
	"$(INTDIR)\DeserHdr.sbr" \
	"$(INTDIR)\DeserMem.sbr" \
	"$(INTDIR)\DeserR.sbr" \
	"$(INTDIR)\DeserR_W.sbr" \
	"$(INTDIR)\DeserSWM.sbr" \
	"$(INTDIR)\DeserSWM_W.sbr" \
	"$(INTDIR)\DesrMHdr.sbr" \
	"$(INTDIR)\DesrMR.sbr" \
	"$(INTDIR)\DesrMSWM.sbr" \
	"$(INTDIR)\SeriaCst.sbr" \
	"$(INTDIR)\SeriaCst_W.sbr" \
	"$(INTDIR)\SeriaHdr.sbr" \
	"$(INTDIR)\SeriaInt.sbr" \
	"$(INTDIR)\Serial.sbr" \
	"$(INTDIR)\Serial_W.sbr" \
	"$(INTDIR)\SerialW.sbr" \
	"$(INTDIR)\SerialW_W.sbr" \
	"$(INTDIR)\SeriaMem.sbr" \
	"$(INTDIR)\SeriaSWM.sbr" \
	"$(INTDIR)\SeriaSWM_W.sbr" \
	"$(INTDIR)\SeriMCst.sbr" \
	"$(INTDIR)\SeriMHdr.sbr" \
	"$(INTDIR)\SeriMSWM.sbr" \
	"$(INTDIR)\SeriMW.sbr"

"$(OUTDIR)\Serialize.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Debug\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\Debug\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"

OUTDIR=.\DebugDll
INTDIR=.\DebugDll
# Begin Custom Macros
OutDir=.\DebugDll
# End Custom Macros

ALL : "..\CommonFiles\DebugDll\KLDTSer.lib" "$(OUTDIR)\Serialize.bsc"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser.sbr"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\Deser_W.sbr"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserH_W.sbr"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserHdr.sbr"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserMem.sbr"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR.sbr"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserR_W.sbr"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM.sbr"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DeserSWM_W.sbr"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMHdr.sbr"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMR.sbr"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\DesrMSWM.sbr"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst.sbr"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaCst_W.sbr"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaHdr.sbr"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\SeriaInt.sbr"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial.sbr"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\Serial_W.sbr"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW.sbr"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SerialW_W.sbr"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaMem.sbr"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM.sbr"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.sbr"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMCst.sbr"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMHdr.sbr"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMSWM.sbr"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\SeriMW.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Serialize.bsc"
	-@erase "..\CommonFiles\DebugDll\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Deser.sbr" \
	"$(INTDIR)\Deser_W.sbr" \
	"$(INTDIR)\DeserH_W.sbr" \
	"$(INTDIR)\DeserHdr.sbr" \
	"$(INTDIR)\DeserMem.sbr" \
	"$(INTDIR)\DeserR.sbr" \
	"$(INTDIR)\DeserR_W.sbr" \
	"$(INTDIR)\DeserSWM.sbr" \
	"$(INTDIR)\DeserSWM_W.sbr" \
	"$(INTDIR)\DesrMHdr.sbr" \
	"$(INTDIR)\DesrMR.sbr" \
	"$(INTDIR)\DesrMSWM.sbr" \
	"$(INTDIR)\SeriaCst.sbr" \
	"$(INTDIR)\SeriaCst_W.sbr" \
	"$(INTDIR)\SeriaHdr.sbr" \
	"$(INTDIR)\SeriaInt.sbr" \
	"$(INTDIR)\Serial.sbr" \
	"$(INTDIR)\Serial_W.sbr" \
	"$(INTDIR)\SerialW.sbr" \
	"$(INTDIR)\SerialW_W.sbr" \
	"$(INTDIR)\SeriaMem.sbr" \
	"$(INTDIR)\SeriaSWM.sbr" \
	"$(INTDIR)\SeriaSWM_W.sbr" \
	"$(INTDIR)\SeriMCst.sbr" \
	"$(INTDIR)\SeriMHdr.sbr" \
	"$(INTDIR)\SeriMSWM.sbr" \
	"$(INTDIR)\SeriMW.sbr"

"$(OUTDIR)\Serialize.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugDll\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\DebugDll\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"

OUTDIR=.\ReleaseDll
INTDIR=.\ReleaseDll

ALL : "..\CommonFiles\ReleaseDll\KLDTSer.lib"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseDll\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseDll\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\ReleaseDll\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"

OUTDIR=.\DebugUDll
INTDIR=.\DebugUDll
# Begin Custom Macros
OutDir=.\DebugUDll
# End Custom Macros

ALL : "..\CommonFiles\DebugUDll\KLDTSer.lib" "$(OUTDIR)\Serialize.bsc"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser.sbr"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\Deser_W.sbr"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserH_W.sbr"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserHdr.sbr"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserMem.sbr"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR.sbr"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserR_W.sbr"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM.sbr"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DeserSWM_W.sbr"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMHdr.sbr"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMR.sbr"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\DesrMSWM.sbr"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst.sbr"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaCst_W.sbr"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaHdr.sbr"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\SeriaInt.sbr"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial.sbr"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\Serial_W.sbr"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW.sbr"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SerialW_W.sbr"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaMem.sbr"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM.sbr"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.sbr"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMCst.sbr"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMHdr.sbr"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMSWM.sbr"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\SeriMW.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Serialize.bsc"
	-@erase "..\CommonFiles\DebugUDll\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Deser.sbr" \
	"$(INTDIR)\Deser_W.sbr" \
	"$(INTDIR)\DeserH_W.sbr" \
	"$(INTDIR)\DeserHdr.sbr" \
	"$(INTDIR)\DeserMem.sbr" \
	"$(INTDIR)\DeserR.sbr" \
	"$(INTDIR)\DeserR_W.sbr" \
	"$(INTDIR)\DeserSWM.sbr" \
	"$(INTDIR)\DeserSWM_W.sbr" \
	"$(INTDIR)\DesrMHdr.sbr" \
	"$(INTDIR)\DesrMR.sbr" \
	"$(INTDIR)\DesrMSWM.sbr" \
	"$(INTDIR)\SeriaCst.sbr" \
	"$(INTDIR)\SeriaCst_W.sbr" \
	"$(INTDIR)\SeriaHdr.sbr" \
	"$(INTDIR)\SeriaInt.sbr" \
	"$(INTDIR)\Serial.sbr" \
	"$(INTDIR)\Serial_W.sbr" \
	"$(INTDIR)\SerialW.sbr" \
	"$(INTDIR)\SerialW_W.sbr" \
	"$(INTDIR)\SeriaMem.sbr" \
	"$(INTDIR)\SeriaSWM.sbr" \
	"$(INTDIR)\SeriaSWM_W.sbr" \
	"$(INTDIR)\SeriMCst.sbr" \
	"$(INTDIR)\SeriMHdr.sbr" \
	"$(INTDIR)\SeriMSWM.sbr" \
	"$(INTDIR)\SeriMW.sbr"

"$(OUTDIR)\Serialize.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugUDll\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\DebugUDll\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"

OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : "..\CommonFiles\ReleaseU\KLDTSer.lib"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseU\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseU\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\ReleaseU\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

ALL : "..\CommonFiles\DebugU\KLDTSer.lib" "$(OUTDIR)\Serialize.bsc"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser.sbr"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\Deser_W.sbr"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserH_W.sbr"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserHdr.sbr"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserMem.sbr"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR.sbr"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserR_W.sbr"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM.sbr"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DeserSWM_W.sbr"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMHdr.sbr"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMR.sbr"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\DesrMSWM.sbr"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst.sbr"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaCst_W.sbr"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaHdr.sbr"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\SeriaInt.sbr"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial.sbr"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\Serial_W.sbr"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW.sbr"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SerialW_W.sbr"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaMem.sbr"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM.sbr"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.sbr"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMCst.sbr"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMHdr.sbr"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMSWM.sbr"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\SeriMW.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Serialize.bsc"
	-@erase "..\CommonFiles\DebugU\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Deser.sbr" \
	"$(INTDIR)\Deser_W.sbr" \
	"$(INTDIR)\DeserH_W.sbr" \
	"$(INTDIR)\DeserHdr.sbr" \
	"$(INTDIR)\DeserMem.sbr" \
	"$(INTDIR)\DeserR.sbr" \
	"$(INTDIR)\DeserR_W.sbr" \
	"$(INTDIR)\DeserSWM.sbr" \
	"$(INTDIR)\DeserSWM_W.sbr" \
	"$(INTDIR)\DesrMHdr.sbr" \
	"$(INTDIR)\DesrMR.sbr" \
	"$(INTDIR)\DesrMSWM.sbr" \
	"$(INTDIR)\SeriaCst.sbr" \
	"$(INTDIR)\SeriaCst_W.sbr" \
	"$(INTDIR)\SeriaHdr.sbr" \
	"$(INTDIR)\SeriaInt.sbr" \
	"$(INTDIR)\Serial.sbr" \
	"$(INTDIR)\Serial_W.sbr" \
	"$(INTDIR)\SerialW.sbr" \
	"$(INTDIR)\SerialW_W.sbr" \
	"$(INTDIR)\SeriaMem.sbr" \
	"$(INTDIR)\SeriaSWM.sbr" \
	"$(INTDIR)\SeriaSWM_W.sbr" \
	"$(INTDIR)\SeriMCst.sbr" \
	"$(INTDIR)\SeriMHdr.sbr" \
	"$(INTDIR)\SeriMSWM.sbr" \
	"$(INTDIR)\SeriMW.sbr"

"$(OUTDIR)\Serialize.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugU\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\DebugU\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"

OUTDIR=.\ReleaseUDll
INTDIR=.\ReleaseUDll

ALL : "..\CommonFiles\ReleaseUDll\KLDTSer.lib"


CLEAN :
	-@erase "$(INTDIR)\Deser.obj"
	-@erase "$(INTDIR)\Deser_W.obj"
	-@erase "$(INTDIR)\DeserH_W.obj"
	-@erase "$(INTDIR)\DeserHdr.obj"
	-@erase "$(INTDIR)\DeserMem.obj"
	-@erase "$(INTDIR)\DeserR.obj"
	-@erase "$(INTDIR)\DeserR_W.obj"
	-@erase "$(INTDIR)\DeserSWM.obj"
	-@erase "$(INTDIR)\DeserSWM_W.obj"
	-@erase "$(INTDIR)\DesrMHdr.obj"
	-@erase "$(INTDIR)\DesrMR.obj"
	-@erase "$(INTDIR)\DesrMSWM.obj"
	-@erase "$(INTDIR)\SeriaCst.obj"
	-@erase "$(INTDIR)\SeriaCst_W.obj"
	-@erase "$(INTDIR)\SeriaHdr.obj"
	-@erase "$(INTDIR)\SeriaInt.obj"
	-@erase "$(INTDIR)\Serial.obj"
	-@erase "$(INTDIR)\Serial_W.obj"
	-@erase "$(INTDIR)\SerialW.obj"
	-@erase "$(INTDIR)\SerialW_W.obj"
	-@erase "$(INTDIR)\SeriaMem.obj"
	-@erase "$(INTDIR)\SeriaSWM.obj"
	-@erase "$(INTDIR)\SeriaSWM_W.obj"
	-@erase "$(INTDIR)\SeriMCst.obj"
	-@erase "$(INTDIR)\SeriMHdr.obj"
	-@erase "$(INTDIR)\SeriMSWM.obj"
	-@erase "$(INTDIR)\SeriMW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseUDll\KLDTSer.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I ".\\" /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Serialize.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseUDll\KLDTSer.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Deser.obj" \
	"$(INTDIR)\Deser_W.obj" \
	"$(INTDIR)\DeserH_W.obj" \
	"$(INTDIR)\DeserHdr.obj" \
	"$(INTDIR)\DeserMem.obj" \
	"$(INTDIR)\DeserR.obj" \
	"$(INTDIR)\DeserR_W.obj" \
	"$(INTDIR)\DeserSWM.obj" \
	"$(INTDIR)\DeserSWM_W.obj" \
	"$(INTDIR)\DesrMHdr.obj" \
	"$(INTDIR)\DesrMR.obj" \
	"$(INTDIR)\DesrMSWM.obj" \
	"$(INTDIR)\SeriaCst.obj" \
	"$(INTDIR)\SeriaCst_W.obj" \
	"$(INTDIR)\SeriaHdr.obj" \
	"$(INTDIR)\SeriaInt.obj" \
	"$(INTDIR)\Serial.obj" \
	"$(INTDIR)\Serial_W.obj" \
	"$(INTDIR)\SerialW.obj" \
	"$(INTDIR)\SerialW_W.obj" \
	"$(INTDIR)\SeriaMem.obj" \
	"$(INTDIR)\SeriaSWM.obj" \
	"$(INTDIR)\SeriaSWM_W.obj" \
	"$(INTDIR)\SeriMCst.obj" \
	"$(INTDIR)\SeriMHdr.obj" \
	"$(INTDIR)\SeriMSWM.obj" \
	"$(INTDIR)\SeriMW.obj"

"..\CommonFiles\ReleaseUDll\KLDTSer.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("Serialize.dep")
!INCLUDE "Serialize.dep"
!ELSE 
!MESSAGE Warning: cannot find "Serialize.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Serialize - Win32 Release" || "$(CFG)" == "Serialize - Win32 Debug" || "$(CFG)" == "Serialize - Win32 DebugDll" || "$(CFG)" == "Serialize - Win32 ReleaseDll" || "$(CFG)" == "Serialize - Win32 Unicode DebugDll" || "$(CFG)" == "Serialize - Win32 Unicode Release" || "$(CFG)" == "Serialize - Win32 Unicode Debug" || "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"
SOURCE=.\Deser.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\Deser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\Deser.obj"	"$(INTDIR)\Deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\Deser.obj"	"$(INTDIR)\Deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\Deser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\Deser.obj"	"$(INTDIR)\Deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\Deser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\Deser.obj"	"$(INTDIR)\Deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\Deser.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Deser_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\Deser_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\Deser_W.obj"	"$(INTDIR)\Deser_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\Deser_W.obj"	"$(INTDIR)\Deser_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\Deser_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\Deser_W.obj"	"$(INTDIR)\Deser_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\Deser_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\Deser_W.obj"	"$(INTDIR)\Deser_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\Deser_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserH_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserH_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserH_W.obj"	"$(INTDIR)\DeserH_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserH_W.obj"	"$(INTDIR)\DeserH_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserH_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserH_W.obj"	"$(INTDIR)\DeserH_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserH_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserH_W.obj"	"$(INTDIR)\DeserH_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserH_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserHdr.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserHdr.obj"	"$(INTDIR)\DeserHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserHdr.obj"	"$(INTDIR)\DeserHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserHdr.obj"	"$(INTDIR)\DeserHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserHdr.obj"	"$(INTDIR)\DeserHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserHdr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserMem.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserMem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserMem.obj"	"$(INTDIR)\DeserMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserMem.obj"	"$(INTDIR)\DeserMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserMem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserMem.obj"	"$(INTDIR)\DeserMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserMem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserMem.obj"	"$(INTDIR)\DeserMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserMem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserR.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserR.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserR.obj"	"$(INTDIR)\DeserR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserR.obj"	"$(INTDIR)\DeserR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserR.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserR.obj"	"$(INTDIR)\DeserR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserR.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserR.obj"	"$(INTDIR)\DeserR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserR.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserR_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserR_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserR_W.obj"	"$(INTDIR)\DeserR_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserR_W.obj"	"$(INTDIR)\DeserR_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserR_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserR_W.obj"	"$(INTDIR)\DeserR_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserR_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserR_W.obj"	"$(INTDIR)\DeserR_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserR_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserSWM.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserSWM.obj"	"$(INTDIR)\DeserSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserSWM.obj"	"$(INTDIR)\DeserSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserSWM.obj"	"$(INTDIR)\DeserSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserSWM.obj"	"$(INTDIR)\DeserSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserSWM.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DeserSWM_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DeserSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DeserSWM_W.obj"	"$(INTDIR)\DeserSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DeserSWM_W.obj"	"$(INTDIR)\DeserSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DeserSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DeserSWM_W.obj"	"$(INTDIR)\DeserSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DeserSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DeserSWM_W.obj"	"$(INTDIR)\DeserSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DeserSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DesrMHdr.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DesrMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DesrMHdr.obj"	"$(INTDIR)\DesrMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DesrMHdr.obj"	"$(INTDIR)\DesrMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DesrMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DesrMHdr.obj"	"$(INTDIR)\DesrMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DesrMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DesrMHdr.obj"	"$(INTDIR)\DesrMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DesrMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DesrMR.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DesrMR.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DesrMR.obj"	"$(INTDIR)\DesrMR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DesrMR.obj"	"$(INTDIR)\DesrMR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DesrMR.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DesrMR.obj"	"$(INTDIR)\DesrMR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DesrMR.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DesrMR.obj"	"$(INTDIR)\DesrMR.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DesrMR.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DesrMSWM.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\DesrMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\DesrMSWM.obj"	"$(INTDIR)\DesrMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\DesrMSWM.obj"	"$(INTDIR)\DesrMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\DesrMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\DesrMSWM.obj"	"$(INTDIR)\DesrMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\DesrMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\DesrMSWM.obj"	"$(INTDIR)\DesrMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\DesrMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaCst.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaCst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaCst.obj"	"$(INTDIR)\SeriaCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaCst.obj"	"$(INTDIR)\SeriaCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaCst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaCst.obj"	"$(INTDIR)\SeriaCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaCst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaCst.obj"	"$(INTDIR)\SeriaCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaCst.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaCst_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaCst_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaCst_W.obj"	"$(INTDIR)\SeriaCst_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaCst_W.obj"	"$(INTDIR)\SeriaCst_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaCst_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaCst_W.obj"	"$(INTDIR)\SeriaCst_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaCst_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaCst_W.obj"	"$(INTDIR)\SeriaCst_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaCst_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaHdr.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaHdr.obj"	"$(INTDIR)\SeriaHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaHdr.obj"	"$(INTDIR)\SeriaHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaHdr.obj"	"$(INTDIR)\SeriaHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaHdr.obj"	"$(INTDIR)\SeriaHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaHdr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaInt.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaInt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaInt.obj"	"$(INTDIR)\SeriaInt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaInt.obj"	"$(INTDIR)\SeriaInt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaInt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaInt.obj"	"$(INTDIR)\SeriaInt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaInt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaInt.obj"	"$(INTDIR)\SeriaInt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaInt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Serial.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\Serial.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\Serial.obj"	"$(INTDIR)\Serial.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\Serial.obj"	"$(INTDIR)\Serial.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\Serial.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\Serial.obj"	"$(INTDIR)\Serial.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\Serial.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\Serial.obj"	"$(INTDIR)\Serial.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\Serial.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Serial_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\Serial_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\Serial_W.obj"	"$(INTDIR)\Serial_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\Serial_W.obj"	"$(INTDIR)\Serial_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\Serial_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\Serial_W.obj"	"$(INTDIR)\Serial_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\Serial_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\Serial_W.obj"	"$(INTDIR)\Serial_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\Serial_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SerialW.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SerialW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SerialW.obj"	"$(INTDIR)\SerialW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SerialW.obj"	"$(INTDIR)\SerialW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SerialW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SerialW.obj"	"$(INTDIR)\SerialW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SerialW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SerialW.obj"	"$(INTDIR)\SerialW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SerialW.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SerialW_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SerialW_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SerialW_W.obj"	"$(INTDIR)\SerialW_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SerialW_W.obj"	"$(INTDIR)\SerialW_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SerialW_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SerialW_W.obj"	"$(INTDIR)\SerialW_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SerialW_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SerialW_W.obj"	"$(INTDIR)\SerialW_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SerialW_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaMem.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaMem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaMem.obj"	"$(INTDIR)\SeriaMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaMem.obj"	"$(INTDIR)\SeriaMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaMem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaMem.obj"	"$(INTDIR)\SeriaMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaMem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaMem.obj"	"$(INTDIR)\SeriaMem.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaMem.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaSWM.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaSWM.obj"	"$(INTDIR)\SeriaSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaSWM.obj"	"$(INTDIR)\SeriaSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaSWM.obj"	"$(INTDIR)\SeriaSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaSWM.obj"	"$(INTDIR)\SeriaSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaSWM.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriaSWM_W.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriaSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriaSWM_W.obj"	"$(INTDIR)\SeriaSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriaSWM_W.obj"	"$(INTDIR)\SeriaSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriaSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriaSWM_W.obj"	"$(INTDIR)\SeriaSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriaSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriaSWM_W.obj"	"$(INTDIR)\SeriaSWM_W.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriaSWM_W.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriMCst.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriMCst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriMCst.obj"	"$(INTDIR)\SeriMCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriMCst.obj"	"$(INTDIR)\SeriMCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriMCst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriMCst.obj"	"$(INTDIR)\SeriMCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriMCst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriMCst.obj"	"$(INTDIR)\SeriMCst.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriMCst.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriMHdr.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriMHdr.obj"	"$(INTDIR)\SeriMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriMHdr.obj"	"$(INTDIR)\SeriMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriMHdr.obj"	"$(INTDIR)\SeriMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriMHdr.obj"	"$(INTDIR)\SeriMHdr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriMHdr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriMSWM.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriMSWM.obj"	"$(INTDIR)\SeriMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriMSWM.obj"	"$(INTDIR)\SeriMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriMSWM.obj"	"$(INTDIR)\SeriMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriMSWM.obj"	"$(INTDIR)\SeriMSWM.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriMSWM.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SeriMW.c

!IF  "$(CFG)" == "Serialize - Win32 Release"


"$(INTDIR)\SeriMW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Debug"


"$(INTDIR)\SeriMW.obj"	"$(INTDIR)\SeriMW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 DebugDll"


"$(INTDIR)\SeriMW.obj"	"$(INTDIR)\SeriMW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 ReleaseDll"


"$(INTDIR)\SeriMW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode DebugDll"


"$(INTDIR)\SeriMW.obj"	"$(INTDIR)\SeriMW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Release"


"$(INTDIR)\SeriMW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode Debug"


"$(INTDIR)\SeriMW.obj"	"$(INTDIR)\SeriMW.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Serialize - Win32 Unicode ReleaseDll"


"$(INTDIR)\SeriMW.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


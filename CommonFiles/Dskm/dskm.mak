# Microsoft Developer Studio Generated NMAKE File, Based on dskm.dsp
!IF "$(CFG)" == ""
CFG=dskm - Win32 DebugDll
!MESSAGE No configuration specified. Defaulting to dskm - Win32 DebugDll.
!ENDIF 

!IF "$(CFG)" != "dskm - Win32 Release" && "$(CFG)" != "dskm - Win32 Debug" && "$(CFG)" != "dskm - Win32 DebugDll" && "$(CFG)" != "dskm - Win32 ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dskm.mak" CFG="dskm - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dskm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dskm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "dskm - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "dskm - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "dskm - Win32 Release"

OUTDIR=.\../../Out/ReleaseS
INTDIR=.\../../Temp/ReleaseS/CommonFiles/dskm
# Begin Custom Macros
OutDir=.\../../Out/ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\dskm.lib" "..\Release\dskm.lib"


CLEAN :
	-@erase "$(INTDIR)\cl_check.obj"
	-@erase "$(INTDIR)\cl_gkey.obj"
	-@erase "$(INTDIR)\cl_hash.obj"
	-@erase "$(INTDIR)\cl_hass.obj"
	-@erase "$(INTDIR)\cl_init.obj"
	-@erase "$(INTDIR)\cl_mkey.obj"
	-@erase "$(INTDIR)\cl_sign.obj"
	-@erase "$(INTDIR)\cl_util.obj"
	-@erase "$(INTDIR)\ds_assch.obj"
	-@erase "$(INTDIR)\ds_assoc.obj"
	-@erase "$(INTDIR)\ds_check.obj"
	-@erase "$(INTDIR)\ds_chkas.obj"
	-@erase "$(INTDIR)\ds_chkbf.obj"
	-@erase "$(INTDIR)\ds_chkfd.obj"
	-@erase "$(INTDIR)\ds_chkfl.obj"
	-@erase "$(INTDIR)\ds_compo.obj"
	-@erase "$(INTDIR)\ds_cregs.obj"
	-@erase "$(INTDIR)\ds_deser.obj"
	-@erase "$(INTDIR)\ds_dump.obj"
	-@erase "$(INTDIR)\ds_erro.obj"
	-@erase "$(INTDIR)\ds_excpt.obj"
	-@erase "$(INTDIR)\ds_ff.obj"
	-@erase "$(INTDIR)\ds_fkey.obj"
	-@erase "$(INTDIR)\ds_gkey.obj"
	-@erase "$(INTDIR)\ds_hash.obj"
	-@erase "$(INTDIR)\ds_init.obj"
	-@erase "$(INTDIR)\ds_inite.obj"
	-@erase "$(INTDIR)\ds_io.obj"
	-@erase "$(INTDIR)\ds_list.obj"
	-@erase "$(INTDIR)\ds_objsb.obj"
	-@erase "$(INTDIR)\ds_objse.obj"
	-@erase "$(INTDIR)\ds_objsn.obj"
	-@erase "$(INTDIR)\ds_objsp.obj"
	-@erase "$(INTDIR)\ds_pars.obj"
	-@erase "$(INTDIR)\ds_parso.obj"
	-@erase "$(INTDIR)\ds_preg.obj"
	-@erase "$(INTDIR)\ds_serial.obj"
	-@erase "$(INTDIR)\ds_utils.obj"
	-@erase "$(INTDIR)\TEXT2BIN.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dskm.lib"
	-@erase "..\Release\dskm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /Zi /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dskm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dskm.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dskm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cl_check.obj" \
	"$(INTDIR)\cl_gkey.obj" \
	"$(INTDIR)\cl_hash.obj" \
	"$(INTDIR)\cl_hass.obj" \
	"$(INTDIR)\cl_init.obj" \
	"$(INTDIR)\cl_mkey.obj" \
	"$(INTDIR)\cl_sign.obj" \
	"$(INTDIR)\cl_util.obj" \
	"$(INTDIR)\ds_assch.obj" \
	"$(INTDIR)\ds_assoc.obj" \
	"$(INTDIR)\ds_check.obj" \
	"$(INTDIR)\ds_chkas.obj" \
	"$(INTDIR)\ds_chkbf.obj" \
	"$(INTDIR)\ds_chkfd.obj" \
	"$(INTDIR)\ds_chkfl.obj" \
	"$(INTDIR)\ds_cregs.obj" \
	"$(INTDIR)\ds_deser.obj" \
	"$(INTDIR)\ds_dump.obj" \
	"$(INTDIR)\ds_erro.obj" \
	"$(INTDIR)\ds_excpt.obj" \
	"$(INTDIR)\ds_ff.obj" \
	"$(INTDIR)\ds_fkey.obj" \
	"$(INTDIR)\ds_gkey.obj" \
	"$(INTDIR)\ds_hash.obj" \
	"$(INTDIR)\ds_init.obj" \
	"$(INTDIR)\ds_inite.obj" \
	"$(INTDIR)\ds_io.obj" \
	"$(INTDIR)\ds_list.obj" \
	"$(INTDIR)\ds_objsb.obj" \
	"$(INTDIR)\ds_objse.obj" \
	"$(INTDIR)\ds_objsn.obj" \
	"$(INTDIR)\ds_objsp.obj" \
	"$(INTDIR)\ds_pars.obj" \
	"$(INTDIR)\ds_parso.obj" \
	"$(INTDIR)\ds_preg.obj" \
	"$(INTDIR)\ds_serial.obj" \
	"$(INTDIR)\ds_utils.obj" \
	"$(INTDIR)\TEXT2BIN.OBJ" \
	".\GOST\Debug\CryptoC.lib" \
	"$(INTDIR)\ds_compo.obj"

"$(OUTDIR)\dskm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseS\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\Release\dskm.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Release
<< 
	

!ELSEIF  "$(CFG)" == "dskm - Win32 Debug"

OUTDIR=.\../../Out/DebugS
INTDIR=.\../../Temp/DebugS/CommonFiles/dskm
# Begin Custom Macros
OutDir=.\../../Out/DebugS
# End Custom Macros

ALL : "$(OUTDIR)\dskm.lib" "..\Debug\dskm.lib"


CLEAN :
	-@erase "$(INTDIR)\cl_check.obj"
	-@erase "$(INTDIR)\cl_gkey.obj"
	-@erase "$(INTDIR)\cl_hash.obj"
	-@erase "$(INTDIR)\cl_hass.obj"
	-@erase "$(INTDIR)\cl_init.obj"
	-@erase "$(INTDIR)\cl_mkey.obj"
	-@erase "$(INTDIR)\cl_sign.obj"
	-@erase "$(INTDIR)\cl_util.obj"
	-@erase "$(INTDIR)\ds_assch.obj"
	-@erase "$(INTDIR)\ds_assoc.obj"
	-@erase "$(INTDIR)\ds_check.obj"
	-@erase "$(INTDIR)\ds_chkas.obj"
	-@erase "$(INTDIR)\ds_chkbf.obj"
	-@erase "$(INTDIR)\ds_chkfd.obj"
	-@erase "$(INTDIR)\ds_chkfl.obj"
	-@erase "$(INTDIR)\ds_compo.obj"
	-@erase "$(INTDIR)\ds_cregs.obj"
	-@erase "$(INTDIR)\ds_deser.obj"
	-@erase "$(INTDIR)\ds_dump.obj"
	-@erase "$(INTDIR)\ds_erro.obj"
	-@erase "$(INTDIR)\ds_excpt.obj"
	-@erase "$(INTDIR)\ds_ff.obj"
	-@erase "$(INTDIR)\ds_fkey.obj"
	-@erase "$(INTDIR)\ds_gkey.obj"
	-@erase "$(INTDIR)\ds_hash.obj"
	-@erase "$(INTDIR)\ds_init.obj"
	-@erase "$(INTDIR)\ds_inite.obj"
	-@erase "$(INTDIR)\ds_io.obj"
	-@erase "$(INTDIR)\ds_list.obj"
	-@erase "$(INTDIR)\ds_objsb.obj"
	-@erase "$(INTDIR)\ds_objse.obj"
	-@erase "$(INTDIR)\ds_objsn.obj"
	-@erase "$(INTDIR)\ds_objsp.obj"
	-@erase "$(INTDIR)\ds_pars.obj"
	-@erase "$(INTDIR)\ds_parso.obj"
	-@erase "$(INTDIR)\ds_preg.obj"
	-@erase "$(INTDIR)\ds_serial.obj"
	-@erase "$(INTDIR)\ds_utils.obj"
	-@erase "$(INTDIR)\TEXT2BIN.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dskm.lib"
	-@erase "..\Debug\dskm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dskm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dskm.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dskm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cl_check.obj" \
	"$(INTDIR)\cl_gkey.obj" \
	"$(INTDIR)\cl_hash.obj" \
	"$(INTDIR)\cl_hass.obj" \
	"$(INTDIR)\cl_init.obj" \
	"$(INTDIR)\cl_mkey.obj" \
	"$(INTDIR)\cl_sign.obj" \
	"$(INTDIR)\cl_util.obj" \
	"$(INTDIR)\ds_assch.obj" \
	"$(INTDIR)\ds_assoc.obj" \
	"$(INTDIR)\ds_check.obj" \
	"$(INTDIR)\ds_chkas.obj" \
	"$(INTDIR)\ds_chkbf.obj" \
	"$(INTDIR)\ds_chkfd.obj" \
	"$(INTDIR)\ds_chkfl.obj" \
	"$(INTDIR)\ds_cregs.obj" \
	"$(INTDIR)\ds_deser.obj" \
	"$(INTDIR)\ds_dump.obj" \
	"$(INTDIR)\ds_erro.obj" \
	"$(INTDIR)\ds_excpt.obj" \
	"$(INTDIR)\ds_ff.obj" \
	"$(INTDIR)\ds_fkey.obj" \
	"$(INTDIR)\ds_gkey.obj" \
	"$(INTDIR)\ds_hash.obj" \
	"$(INTDIR)\ds_init.obj" \
	"$(INTDIR)\ds_inite.obj" \
	"$(INTDIR)\ds_io.obj" \
	"$(INTDIR)\ds_list.obj" \
	"$(INTDIR)\ds_objsb.obj" \
	"$(INTDIR)\ds_objse.obj" \
	"$(INTDIR)\ds_objsn.obj" \
	"$(INTDIR)\ds_objsp.obj" \
	"$(INTDIR)\ds_pars.obj" \
	"$(INTDIR)\ds_parso.obj" \
	"$(INTDIR)\ds_preg.obj" \
	"$(INTDIR)\ds_serial.obj" \
	"$(INTDIR)\ds_utils.obj" \
	"$(INTDIR)\TEXT2BIN.OBJ" \
	".\GOST\Debug\CryptoC.lib" \
	"$(INTDIR)\ds_compo.obj"

"$(OUTDIR)\dskm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugS\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\Debug\dskm.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "dskm - Win32 DebugDll"

OUTDIR=.\../../Out/Debug
INTDIR=.\../../Temp/Debug/CommonFiles/dskm
# Begin Custom Macros
OutDir=.\../../Out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\dskm.lib" "..\DebugDll\dskm.lib"


CLEAN :
	-@erase "$(INTDIR)\cl_check.obj"
	-@erase "$(INTDIR)\cl_gkey.obj"
	-@erase "$(INTDIR)\cl_hash.obj"
	-@erase "$(INTDIR)\cl_hass.obj"
	-@erase "$(INTDIR)\cl_init.obj"
	-@erase "$(INTDIR)\cl_mkey.obj"
	-@erase "$(INTDIR)\cl_sign.obj"
	-@erase "$(INTDIR)\cl_util.obj"
	-@erase "$(INTDIR)\ds_assch.obj"
	-@erase "$(INTDIR)\ds_assoc.obj"
	-@erase "$(INTDIR)\ds_check.obj"
	-@erase "$(INTDIR)\ds_chkas.obj"
	-@erase "$(INTDIR)\ds_chkbf.obj"
	-@erase "$(INTDIR)\ds_chkfd.obj"
	-@erase "$(INTDIR)\ds_chkfl.obj"
	-@erase "$(INTDIR)\ds_compo.obj"
	-@erase "$(INTDIR)\ds_cregs.obj"
	-@erase "$(INTDIR)\ds_deser.obj"
	-@erase "$(INTDIR)\ds_dump.obj"
	-@erase "$(INTDIR)\ds_erro.obj"
	-@erase "$(INTDIR)\ds_excpt.obj"
	-@erase "$(INTDIR)\ds_ff.obj"
	-@erase "$(INTDIR)\ds_fkey.obj"
	-@erase "$(INTDIR)\ds_gkey.obj"
	-@erase "$(INTDIR)\ds_hash.obj"
	-@erase "$(INTDIR)\ds_init.obj"
	-@erase "$(INTDIR)\ds_inite.obj"
	-@erase "$(INTDIR)\ds_io.obj"
	-@erase "$(INTDIR)\ds_list.obj"
	-@erase "$(INTDIR)\ds_objsb.obj"
	-@erase "$(INTDIR)\ds_objse.obj"
	-@erase "$(INTDIR)\ds_objsn.obj"
	-@erase "$(INTDIR)\ds_objsp.obj"
	-@erase "$(INTDIR)\ds_pars.obj"
	-@erase "$(INTDIR)\ds_parso.obj"
	-@erase "$(INTDIR)\ds_preg.obj"
	-@erase "$(INTDIR)\ds_serial.obj"
	-@erase "$(INTDIR)\ds_utils.obj"
	-@erase "$(INTDIR)\TEXT2BIN.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dskm.lib"
	-@erase "..\DebugDll\dskm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dskm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dskm.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dskm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cl_check.obj" \
	"$(INTDIR)\cl_gkey.obj" \
	"$(INTDIR)\cl_hash.obj" \
	"$(INTDIR)\cl_hass.obj" \
	"$(INTDIR)\cl_init.obj" \
	"$(INTDIR)\cl_mkey.obj" \
	"$(INTDIR)\cl_sign.obj" \
	"$(INTDIR)\cl_util.obj" \
	"$(INTDIR)\ds_assch.obj" \
	"$(INTDIR)\ds_assoc.obj" \
	"$(INTDIR)\ds_check.obj" \
	"$(INTDIR)\ds_chkas.obj" \
	"$(INTDIR)\ds_chkbf.obj" \
	"$(INTDIR)\ds_chkfd.obj" \
	"$(INTDIR)\ds_chkfl.obj" \
	"$(INTDIR)\ds_cregs.obj" \
	"$(INTDIR)\ds_deser.obj" \
	"$(INTDIR)\ds_dump.obj" \
	"$(INTDIR)\ds_erro.obj" \
	"$(INTDIR)\ds_excpt.obj" \
	"$(INTDIR)\ds_ff.obj" \
	"$(INTDIR)\ds_fkey.obj" \
	"$(INTDIR)\ds_gkey.obj" \
	"$(INTDIR)\ds_hash.obj" \
	"$(INTDIR)\ds_init.obj" \
	"$(INTDIR)\ds_inite.obj" \
	"$(INTDIR)\ds_io.obj" \
	"$(INTDIR)\ds_list.obj" \
	"$(INTDIR)\ds_objsb.obj" \
	"$(INTDIR)\ds_objse.obj" \
	"$(INTDIR)\ds_objsn.obj" \
	"$(INTDIR)\ds_objsp.obj" \
	"$(INTDIR)\ds_pars.obj" \
	"$(INTDIR)\ds_parso.obj" \
	"$(INTDIR)\ds_preg.obj" \
	"$(INTDIR)\ds_serial.obj" \
	"$(INTDIR)\ds_utils.obj" \
	"$(INTDIR)\TEXT2BIN.OBJ" \
	".\GOST\Debug\CryptoC.lib" \
	"$(INTDIR)\ds_compo.obj"

"$(OUTDIR)\dskm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Debug\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\DebugDll\dskm.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "dskm - Win32 ReleaseDll"

OUTDIR=.\../../Out/Release
INTDIR=.\../../Temp/Release/CommonFiles/dskm
# Begin Custom Macros
OutDir=.\../../Out/Release
# End Custom Macros

ALL : "$(OUTDIR)\dskm.lib" "..\ReleaseDll\dskm.lib"


CLEAN :
	-@erase "$(INTDIR)\cl_check.obj"
	-@erase "$(INTDIR)\cl_gkey.obj"
	-@erase "$(INTDIR)\cl_hash.obj"
	-@erase "$(INTDIR)\cl_hass.obj"
	-@erase "$(INTDIR)\cl_init.obj"
	-@erase "$(INTDIR)\cl_mkey.obj"
	-@erase "$(INTDIR)\cl_sign.obj"
	-@erase "$(INTDIR)\cl_util.obj"
	-@erase "$(INTDIR)\ds_assch.obj"
	-@erase "$(INTDIR)\ds_assoc.obj"
	-@erase "$(INTDIR)\ds_check.obj"
	-@erase "$(INTDIR)\ds_chkas.obj"
	-@erase "$(INTDIR)\ds_chkbf.obj"
	-@erase "$(INTDIR)\ds_chkfd.obj"
	-@erase "$(INTDIR)\ds_chkfl.obj"
	-@erase "$(INTDIR)\ds_compo.obj"
	-@erase "$(INTDIR)\ds_cregs.obj"
	-@erase "$(INTDIR)\ds_deser.obj"
	-@erase "$(INTDIR)\ds_dump.obj"
	-@erase "$(INTDIR)\ds_erro.obj"
	-@erase "$(INTDIR)\ds_excpt.obj"
	-@erase "$(INTDIR)\ds_ff.obj"
	-@erase "$(INTDIR)\ds_fkey.obj"
	-@erase "$(INTDIR)\ds_gkey.obj"
	-@erase "$(INTDIR)\ds_hash.obj"
	-@erase "$(INTDIR)\ds_init.obj"
	-@erase "$(INTDIR)\ds_inite.obj"
	-@erase "$(INTDIR)\ds_io.obj"
	-@erase "$(INTDIR)\ds_list.obj"
	-@erase "$(INTDIR)\ds_objsb.obj"
	-@erase "$(INTDIR)\ds_objse.obj"
	-@erase "$(INTDIR)\ds_objsn.obj"
	-@erase "$(INTDIR)\ds_objsp.obj"
	-@erase "$(INTDIR)\ds_pars.obj"
	-@erase "$(INTDIR)\ds_parso.obj"
	-@erase "$(INTDIR)\ds_preg.obj"
	-@erase "$(INTDIR)\ds_serial.obj"
	-@erase "$(INTDIR)\ds_utils.obj"
	-@erase "$(INTDIR)\TEXT2BIN.OBJ"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dskm.lib"
	-@erase "..\ReleaseDll\dskm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /Ox /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\dskm.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dskm.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dskm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cl_check.obj" \
	"$(INTDIR)\cl_gkey.obj" \
	"$(INTDIR)\cl_hash.obj" \
	"$(INTDIR)\cl_hass.obj" \
	"$(INTDIR)\cl_init.obj" \
	"$(INTDIR)\cl_mkey.obj" \
	"$(INTDIR)\cl_sign.obj" \
	"$(INTDIR)\cl_util.obj" \
	"$(INTDIR)\ds_assch.obj" \
	"$(INTDIR)\ds_assoc.obj" \
	"$(INTDIR)\ds_check.obj" \
	"$(INTDIR)\ds_chkas.obj" \
	"$(INTDIR)\ds_chkbf.obj" \
	"$(INTDIR)\ds_chkfd.obj" \
	"$(INTDIR)\ds_chkfl.obj" \
	"$(INTDIR)\ds_cregs.obj" \
	"$(INTDIR)\ds_deser.obj" \
	"$(INTDIR)\ds_dump.obj" \
	"$(INTDIR)\ds_erro.obj" \
	"$(INTDIR)\ds_excpt.obj" \
	"$(INTDIR)\ds_ff.obj" \
	"$(INTDIR)\ds_fkey.obj" \
	"$(INTDIR)\ds_gkey.obj" \
	"$(INTDIR)\ds_hash.obj" \
	"$(INTDIR)\ds_init.obj" \
	"$(INTDIR)\ds_inite.obj" \
	"$(INTDIR)\ds_io.obj" \
	"$(INTDIR)\ds_list.obj" \
	"$(INTDIR)\ds_objsb.obj" \
	"$(INTDIR)\ds_objse.obj" \
	"$(INTDIR)\ds_objsn.obj" \
	"$(INTDIR)\ds_objsp.obj" \
	"$(INTDIR)\ds_pars.obj" \
	"$(INTDIR)\ds_parso.obj" \
	"$(INTDIR)\ds_preg.obj" \
	"$(INTDIR)\ds_serial.obj" \
	"$(INTDIR)\ds_utils.obj" \
	"$(INTDIR)\TEXT2BIN.OBJ" \
	".\GOST\Debug\CryptoC.lib" \
	"$(INTDIR)\ds_compo.obj"

"$(OUTDIR)\dskm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Release\dskm.lib
InputName=dskm
SOURCE="$(InputPath)"

"..\ReleaseDll\dskm.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\ReleaseDll
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
!IF EXISTS("dskm.dep")
!INCLUDE "dskm.dep"
!ELSE 
!MESSAGE Warning: cannot find "dskm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dskm - Win32 Release" || "$(CFG)" == "dskm - Win32 Debug" || "$(CFG)" == "dskm - Win32 DebugDll" || "$(CFG)" == "dskm - Win32 ReleaseDll"
SOURCE=.\cl_check.c

"$(INTDIR)\cl_check.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_gkey.c

"$(INTDIR)\cl_gkey.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_hash.c

"$(INTDIR)\cl_hash.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_hass.c

"$(INTDIR)\cl_hass.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_init.c

"$(INTDIR)\cl_init.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_mkey.c

"$(INTDIR)\cl_mkey.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_sign.c

"$(INTDIR)\cl_sign.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cl_util.c

"$(INTDIR)\cl_util.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_assch.c

"$(INTDIR)\ds_assch.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_assoc.c

"$(INTDIR)\ds_assoc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_check.c

"$(INTDIR)\ds_check.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_chkas.c

"$(INTDIR)\ds_chkas.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_chkbf.c

"$(INTDIR)\ds_chkbf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_chkfd.c

"$(INTDIR)\ds_chkfd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_chkfl.c

"$(INTDIR)\ds_chkfl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_compo.c

"$(INTDIR)\ds_compo.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_cregs.c

"$(INTDIR)\ds_cregs.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_deser.c

"$(INTDIR)\ds_deser.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_dump.c

"$(INTDIR)\ds_dump.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_erro.c

"$(INTDIR)\ds_erro.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_excpt.c

"$(INTDIR)\ds_excpt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_ff.c

"$(INTDIR)\ds_ff.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_fkey.c

"$(INTDIR)\ds_fkey.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_gkey.c

"$(INTDIR)\ds_gkey.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_hash.c

"$(INTDIR)\ds_hash.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_init.c

"$(INTDIR)\ds_init.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_inite.c

"$(INTDIR)\ds_inite.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_io.c

"$(INTDIR)\ds_io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_list.c

"$(INTDIR)\ds_list.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_objsb.c

"$(INTDIR)\ds_objsb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_objse.c

"$(INTDIR)\ds_objse.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_objsn.c

"$(INTDIR)\ds_objsn.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_objsp.c

"$(INTDIR)\ds_objsp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_pars.c

"$(INTDIR)\ds_pars.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_parso.c

"$(INTDIR)\ds_parso.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_preg.c

"$(INTDIR)\ds_preg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_serial.c

"$(INTDIR)\ds_serial.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ds_utils.c

"$(INTDIR)\ds_utils.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\Stuff\TEXT2BIN.C

"$(INTDIR)\TEXT2BIN.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 


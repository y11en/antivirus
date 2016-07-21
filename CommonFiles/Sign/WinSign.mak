# Microsoft Developer Studio Generated NMAKE File, Based on WinSign.dsp
!IF "$(CFG)" == ""
CFG=WinSign - Win32 DebugDll With Debug Key
!MESSAGE No configuration specified. Defaulting to WinSign - Win32 DebugDll With Debug Key.
!ENDIF 

!IF "$(CFG)" != "WinSign - Win32 Release" && "$(CFG)" != "WinSign - Win32 Debug" && "$(CFG)" != "WinSign - Win32 DebugDll" && "$(CFG)" != "WinSign - Win32 ReleaseDll" && "$(CFG)" != "WinSign - Win32 Debug With Debug Key" && "$(CFG)" != "WinSign - Win32 DebugDll With Debug Key" && "$(CFG)" != "WinSign - Win32 Novell_Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinSign.mak" CFG="WinSign - Win32 DebugDll With Debug Key"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinSign - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 Debug With Debug Key" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 DebugDll With Debug Key" (based on "Win32 (x86) Static Library")
!MESSAGE "WinSign - Win32 Novell_Release" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "WinSign - Win32 Release"

OUTDIR=.\../../Out/ReleaseS
INTDIR=.\../../Temp/ReleaseS/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\Sign.lib" "..\Release\Sign.lib"


CLEAN :
	-@erase "$(INTDIR)\errno.obj"
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_chfile.obj"
	-@erase "$(INTDIR)\s_chfilw.obj"
	-@erase "$(INTDIR)\s_chfres.obj"
	-@erase "$(INTDIR)\s_chres.obj"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_crpriv.obj"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_lnfile.obj"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_putn.obj"
	-@erase "$(INTDIR)\s_sgnfil.obj"
	-@erase "$(INTDIR)\s_sgnmem.obj"
	-@erase "$(INTDIR)\s_sifile.obj"
	-@erase "$(INTDIR)\s_sign.obj"
	-@erase "$(INTDIR)\s_spkey.obj"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Sign.lib"
	-@erase "..\Release\Sign.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GR /GX /O2 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /Fp"$(INTDIR)\WinSign.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinSign.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign.lib" /LINK50COMPAT 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_srand.obj" \
	".\objs\c_tm.obj" \
	".\objs\c_urand.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\f_sign.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_file.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_gostrf.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_privat.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	"$(INTDIR)\errno.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_chfile.obj" \
	"$(INTDIR)\s_chfilw.obj" \
	"$(INTDIR)\s_chfres.obj" \
	"$(INTDIR)\s_chres.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_crpriv.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnfile.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_putn.obj" \
	"$(INTDIR)\s_sgnfil.obj" \
	"$(INTDIR)\s_sgnmem.obj" \
	"$(INTDIR)\s_sifile.obj" \
	"$(INTDIR)\s_sign.obj" \
	"$(INTDIR)\s_spkey.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseS\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\Release\Sign.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Release
<< 
	

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

OUTDIR=.\../../Out/DebugS
INTDIR=.\../../Temp/DebugS/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/DebugS
# End Custom Macros

ALL : "$(OUTDIR)\Sign.lib" ".\Debug\WinSign.bsc" "..\Debug\Sign.lib"


CLEAN :
	-@erase "$(INTDIR)\errno.obj"
	-@erase "$(INTDIR)\errno.sbr"
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\my_strl.sbr"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\n_data.sbr"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_buflen.sbr"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_chbuf.sbr"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_check.sbr"
	-@erase "$(INTDIR)\s_chfile.obj"
	-@erase "$(INTDIR)\s_chfile.sbr"
	-@erase "$(INTDIR)\s_chfilw.obj"
	-@erase "$(INTDIR)\s_chfilw.sbr"
	-@erase "$(INTDIR)\s_chfres.obj"
	-@erase "$(INTDIR)\s_chfres.sbr"
	-@erase "$(INTDIR)\s_chres.obj"
	-@erase "$(INTDIR)\s_chres.sbr"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_cnot.sbr"
	-@erase "$(INTDIR)\s_crpriv.obj"
	-@erase "$(INTDIR)\s_crpriv.sbr"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_except.sbr"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnguid.sbr"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_fnstr.sbr"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_hash.sbr"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_klok.sbr"
	-@erase "$(INTDIR)\s_lnfile.obj"
	-@erase "$(INTDIR)\s_lnfile.sbr"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_lnot.sbr"
	-@erase "$(INTDIR)\s_putn.obj"
	-@erase "$(INTDIR)\s_putn.sbr"
	-@erase "$(INTDIR)\s_sgnfil.obj"
	-@erase "$(INTDIR)\s_sgnfil.sbr"
	-@erase "$(INTDIR)\s_sgnmem.obj"
	-@erase "$(INTDIR)\s_sgnmem.sbr"
	-@erase "$(INTDIR)\s_sifile.obj"
	-@erase "$(INTDIR)\s_sifile.sbr"
	-@erase "$(INTDIR)\s_sign.obj"
	-@erase "$(INTDIR)\s_sign.sbr"
	-@erase "$(INTDIR)\s_spkey.obj"
	-@erase "$(INTDIR)\s_spkey.sbr"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\s_strver.sbr"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\text2bin.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Sign.lib"
	-@erase ".\Debug\WinSign.bsc"
	-@erase "..\Debug\Sign.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"Debug/WinSign.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\errno.sbr" \
	"$(INTDIR)\my_strl.sbr" \
	"$(INTDIR)\n_data.sbr" \
	"$(INTDIR)\s_buflen.sbr" \
	"$(INTDIR)\s_chbuf.sbr" \
	"$(INTDIR)\s_check.sbr" \
	"$(INTDIR)\s_chfile.sbr" \
	"$(INTDIR)\s_chfilw.sbr" \
	"$(INTDIR)\s_chfres.sbr" \
	"$(INTDIR)\s_chres.sbr" \
	"$(INTDIR)\s_cnot.sbr" \
	"$(INTDIR)\s_crpriv.sbr" \
	"$(INTDIR)\s_except.sbr" \
	"$(INTDIR)\s_fnguid.sbr" \
	"$(INTDIR)\s_fnstr.sbr" \
	"$(INTDIR)\s_hash.sbr" \
	"$(INTDIR)\s_klok.sbr" \
	"$(INTDIR)\s_lnfile.sbr" \
	"$(INTDIR)\s_lnot.sbr" \
	"$(INTDIR)\s_putn.sbr" \
	"$(INTDIR)\s_sgnfil.sbr" \
	"$(INTDIR)\s_sgnmem.sbr" \
	"$(INTDIR)\s_sifile.sbr" \
	"$(INTDIR)\s_sign.sbr" \
	"$(INTDIR)\s_spkey.sbr" \
	"$(INTDIR)\s_strver.sbr" \
	"$(INTDIR)\text2bin.sbr"

".\Debug\WinSign.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign.lib" 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_srand.obj" \
	".\objs\c_tm.obj" \
	".\objs\c_urand.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\f_sign.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_file.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_gostrf.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_privat.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	"$(INTDIR)\errno.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_chfile.obj" \
	"$(INTDIR)\s_chfilw.obj" \
	"$(INTDIR)\s_chfres.obj" \
	"$(INTDIR)\s_chres.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_crpriv.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnfile.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_putn.obj" \
	"$(INTDIR)\s_sgnfil.obj" \
	"$(INTDIR)\s_sgnmem.obj" \
	"$(INTDIR)\s_sifile.obj" \
	"$(INTDIR)\s_sign.obj" \
	"$(INTDIR)\s_spkey.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugS\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\Debug\Sign.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

OUTDIR=.\../../Out/Debug
INTDIR=.\../../Temp/Debug/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\Sign.lib" ".\DebugDll\WinSign.bsc" "..\DebugDll\Sign.lib"


CLEAN :
	-@erase "$(INTDIR)\errno.obj"
	-@erase "$(INTDIR)\errno.sbr"
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\my_strl.sbr"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\n_data.sbr"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_buflen.sbr"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_chbuf.sbr"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_check.sbr"
	-@erase "$(INTDIR)\s_chfile.obj"
	-@erase "$(INTDIR)\s_chfile.sbr"
	-@erase "$(INTDIR)\s_chfilw.obj"
	-@erase "$(INTDIR)\s_chfilw.sbr"
	-@erase "$(INTDIR)\s_chfres.obj"
	-@erase "$(INTDIR)\s_chfres.sbr"
	-@erase "$(INTDIR)\s_chres.obj"
	-@erase "$(INTDIR)\s_chres.sbr"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_cnot.sbr"
	-@erase "$(INTDIR)\s_crpriv.obj"
	-@erase "$(INTDIR)\s_crpriv.sbr"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_except.sbr"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnguid.sbr"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_fnstr.sbr"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_hash.sbr"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_klok.sbr"
	-@erase "$(INTDIR)\s_lnfile.obj"
	-@erase "$(INTDIR)\s_lnfile.sbr"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_lnot.sbr"
	-@erase "$(INTDIR)\s_putn.obj"
	-@erase "$(INTDIR)\s_putn.sbr"
	-@erase "$(INTDIR)\s_sgnfil.obj"
	-@erase "$(INTDIR)\s_sgnfil.sbr"
	-@erase "$(INTDIR)\s_sgnmem.obj"
	-@erase "$(INTDIR)\s_sgnmem.sbr"
	-@erase "$(INTDIR)\s_sifile.obj"
	-@erase "$(INTDIR)\s_sifile.sbr"
	-@erase "$(INTDIR)\s_sign.obj"
	-@erase "$(INTDIR)\s_sign.sbr"
	-@erase "$(INTDIR)\s_spkey.obj"
	-@erase "$(INTDIR)\s_spkey.sbr"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\s_strver.sbr"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\text2bin.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Sign.lib"
	-@erase ".\DebugDll\WinSign.bsc"
	-@erase "..\DebugDll\Sign.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"DebugDll/WinSign.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\errno.sbr" \
	"$(INTDIR)\my_strl.sbr" \
	"$(INTDIR)\n_data.sbr" \
	"$(INTDIR)\s_buflen.sbr" \
	"$(INTDIR)\s_chbuf.sbr" \
	"$(INTDIR)\s_check.sbr" \
	"$(INTDIR)\s_chfile.sbr" \
	"$(INTDIR)\s_chfilw.sbr" \
	"$(INTDIR)\s_chfres.sbr" \
	"$(INTDIR)\s_chres.sbr" \
	"$(INTDIR)\s_cnot.sbr" \
	"$(INTDIR)\s_crpriv.sbr" \
	"$(INTDIR)\s_except.sbr" \
	"$(INTDIR)\s_fnguid.sbr" \
	"$(INTDIR)\s_fnstr.sbr" \
	"$(INTDIR)\s_hash.sbr" \
	"$(INTDIR)\s_klok.sbr" \
	"$(INTDIR)\s_lnfile.sbr" \
	"$(INTDIR)\s_lnot.sbr" \
	"$(INTDIR)\s_putn.sbr" \
	"$(INTDIR)\s_sgnfil.sbr" \
	"$(INTDIR)\s_sgnmem.sbr" \
	"$(INTDIR)\s_sifile.sbr" \
	"$(INTDIR)\s_sign.sbr" \
	"$(INTDIR)\s_spkey.sbr" \
	"$(INTDIR)\s_strver.sbr" \
	"$(INTDIR)\text2bin.sbr"

".\DebugDll\WinSign.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign.lib" 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_srand.obj" \
	".\objs\c_swdict.obj" \
	".\objs\c_tm.obj" \
	".\objs\c_urand.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\f_sign.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_file.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_gostrf.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_privat.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	"$(INTDIR)\errno.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_chfile.obj" \
	"$(INTDIR)\s_chfilw.obj" \
	"$(INTDIR)\s_chfres.obj" \
	"$(INTDIR)\s_chres.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_crpriv.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnfile.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_putn.obj" \
	"$(INTDIR)\s_sgnfil.obj" \
	"$(INTDIR)\s_sgnmem.obj" \
	"$(INTDIR)\s_sifile.obj" \
	"$(INTDIR)\s_sign.obj" \
	"$(INTDIR)\s_spkey.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Debug\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\DebugDll\Sign.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

OUTDIR=.\../../Out/Release
INTDIR=.\../../Temp/Release/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/Release
# End Custom Macros

ALL : "$(OUTDIR)\Sign.lib" "..\ReleaseDll\Sign.lib"


CLEAN :
	-@erase "$(INTDIR)\errno.obj"
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_chfile.obj"
	-@erase "$(INTDIR)\s_chfilw.obj"
	-@erase "$(INTDIR)\s_chfres.obj"
	-@erase "$(INTDIR)\s_chres.obj"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_crpriv.obj"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_lnfile.obj"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_putn.obj"
	-@erase "$(INTDIR)\s_sgnfil.obj"
	-@erase "$(INTDIR)\s_sgnmem.obj"
	-@erase "$(INTDIR)\s_sifile.obj"
	-@erase "$(INTDIR)\s_sign.obj"
	-@erase "$(INTDIR)\s_spkey.obj"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Sign.lib"
	-@erase "..\ReleaseDll\Sign.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GR /GX /O2 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /Fp"$(INTDIR)\WinSign.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinSign.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign.lib" /LINK50COMPAT 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_srand.obj" \
	".\objs\c_swdict.obj" \
	".\objs\c_tm.obj" \
	".\objs\c_urand.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\f_sign.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_file.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_gostrf.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_privat.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	"$(INTDIR)\errno.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_chfile.obj" \
	"$(INTDIR)\s_chfilw.obj" \
	"$(INTDIR)\s_chfres.obj" \
	"$(INTDIR)\s_chres.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_crpriv.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnfile.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_putn.obj" \
	"$(INTDIR)\s_sgnfil.obj" \
	"$(INTDIR)\s_sgnmem.obj" \
	"$(INTDIR)\s_sifile.obj" \
	"$(INTDIR)\s_sign.obj" \
	"$(INTDIR)\s_spkey.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Release\Sign.lib
InputName=Sign
SOURCE="$(InputPath)"

"..\ReleaseDll\Sign.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\ReleaseDll
<< 
	

!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

OUTDIR=.\../../Out/DebugS
INTDIR=.\../../Temp/DebugWDKey/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/DebugS
# End Custom Macros

ALL : "$(OUTDIR)\Sign_wdk.lib" ".\Debug\Sign_wdk.bsc" "..\Debug\Sign_wdk.lib"


CLEAN :
	-@erase "$(INTDIR)\errno.obj"
	-@erase "$(INTDIR)\errno.sbr"
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\my_strl.sbr"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\n_data.sbr"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_buflen.sbr"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_chbuf.sbr"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_check.sbr"
	-@erase "$(INTDIR)\s_chfile.obj"
	-@erase "$(INTDIR)\s_chfile.sbr"
	-@erase "$(INTDIR)\s_chfilw.obj"
	-@erase "$(INTDIR)\s_chfilw.sbr"
	-@erase "$(INTDIR)\s_chfres.obj"
	-@erase "$(INTDIR)\s_chfres.sbr"
	-@erase "$(INTDIR)\s_chres.obj"
	-@erase "$(INTDIR)\s_chres.sbr"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_cnot.sbr"
	-@erase "$(INTDIR)\s_crpriv.obj"
	-@erase "$(INTDIR)\s_crpriv.sbr"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_except.sbr"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnguid.sbr"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_fnstr.sbr"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_hash.sbr"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_klok.sbr"
	-@erase "$(INTDIR)\s_lnfile.obj"
	-@erase "$(INTDIR)\s_lnfile.sbr"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_lnot.sbr"
	-@erase "$(INTDIR)\s_putn.obj"
	-@erase "$(INTDIR)\s_putn.sbr"
	-@erase "$(INTDIR)\s_sgnfil.obj"
	-@erase "$(INTDIR)\s_sgnfil.sbr"
	-@erase "$(INTDIR)\s_sgnmem.obj"
	-@erase "$(INTDIR)\s_sgnmem.sbr"
	-@erase "$(INTDIR)\s_sifile.obj"
	-@erase "$(INTDIR)\s_sifile.sbr"
	-@erase "$(INTDIR)\s_sign.obj"
	-@erase "$(INTDIR)\s_sign.sbr"
	-@erase "$(INTDIR)\s_spkey.obj"
	-@erase "$(INTDIR)\s_spkey.sbr"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\s_strver.sbr"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\text2bin.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Sign_wdk.lib"
	-@erase ".\Debug\Sign_wdk.bsc"
	-@erase "..\Debug\Sign_wdk.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"Debug/Sign_wdk.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\errno.sbr" \
	"$(INTDIR)\my_strl.sbr" \
	"$(INTDIR)\n_data.sbr" \
	"$(INTDIR)\s_buflen.sbr" \
	"$(INTDIR)\s_chbuf.sbr" \
	"$(INTDIR)\s_check.sbr" \
	"$(INTDIR)\s_chfile.sbr" \
	"$(INTDIR)\s_chfilw.sbr" \
	"$(INTDIR)\s_chfres.sbr" \
	"$(INTDIR)\s_chres.sbr" \
	"$(INTDIR)\s_cnot.sbr" \
	"$(INTDIR)\s_crpriv.sbr" \
	"$(INTDIR)\s_except.sbr" \
	"$(INTDIR)\s_fnguid.sbr" \
	"$(INTDIR)\s_fnstr.sbr" \
	"$(INTDIR)\s_hash.sbr" \
	"$(INTDIR)\s_klok.sbr" \
	"$(INTDIR)\s_lnfile.sbr" \
	"$(INTDIR)\s_lnot.sbr" \
	"$(INTDIR)\s_putn.sbr" \
	"$(INTDIR)\s_sgnfil.sbr" \
	"$(INTDIR)\s_sgnmem.sbr" \
	"$(INTDIR)\s_sifile.sbr" \
	"$(INTDIR)\s_sign.sbr" \
	"$(INTDIR)\s_spkey.sbr" \
	"$(INTDIR)\s_strver.sbr" \
	"$(INTDIR)\text2bin.sbr"

".\Debug\Sign_wdk.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign_wdk.lib" 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_srand.obj" \
	".\objs\c_tm.obj" \
	".\objs\c_urand.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\f_sign.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_file.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_gostrf.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_privat.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	"$(INTDIR)\errno.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_chfile.obj" \
	"$(INTDIR)\s_chfilw.obj" \
	"$(INTDIR)\s_chfres.obj" \
	"$(INTDIR)\s_chres.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_crpriv.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnfile.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_putn.obj" \
	"$(INTDIR)\s_sgnfil.obj" \
	"$(INTDIR)\s_sgnmem.obj" \
	"$(INTDIR)\s_sifile.obj" \
	"$(INTDIR)\s_sign.obj" \
	"$(INTDIR)\s_spkey.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign_wdk.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugS\Sign_wdk.lib
InputName=Sign_wdk
SOURCE="$(InputPath)"

"..\Debug\Sign_wdk.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

OUTDIR=.\../../Out/Debug
INTDIR=.\../../Temp/DebugDllWDKey/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\Sign_wdk.lib" ".\DebugDll\Sign_wdk.bsc" "..\DebugDll\Sign_wdk.lib"


CLEAN :
	-@erase "$(INTDIR)\errno.obj"
	-@erase "$(INTDIR)\errno.sbr"
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\my_strl.sbr"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\n_data.sbr"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_buflen.sbr"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_chbuf.sbr"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_check.sbr"
	-@erase "$(INTDIR)\s_chfile.obj"
	-@erase "$(INTDIR)\s_chfile.sbr"
	-@erase "$(INTDIR)\s_chfilw.obj"
	-@erase "$(INTDIR)\s_chfilw.sbr"
	-@erase "$(INTDIR)\s_chfres.obj"
	-@erase "$(INTDIR)\s_chfres.sbr"
	-@erase "$(INTDIR)\s_chres.obj"
	-@erase "$(INTDIR)\s_chres.sbr"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_cnot.sbr"
	-@erase "$(INTDIR)\s_crpriv.obj"
	-@erase "$(INTDIR)\s_crpriv.sbr"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_except.sbr"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnguid.sbr"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_fnstr.sbr"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_hash.sbr"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_klok.sbr"
	-@erase "$(INTDIR)\s_lnfile.obj"
	-@erase "$(INTDIR)\s_lnfile.sbr"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_lnot.sbr"
	-@erase "$(INTDIR)\s_putn.obj"
	-@erase "$(INTDIR)\s_putn.sbr"
	-@erase "$(INTDIR)\s_sgnfil.obj"
	-@erase "$(INTDIR)\s_sgnfil.sbr"
	-@erase "$(INTDIR)\s_sgnmem.obj"
	-@erase "$(INTDIR)\s_sgnmem.sbr"
	-@erase "$(INTDIR)\s_sifile.obj"
	-@erase "$(INTDIR)\s_sifile.sbr"
	-@erase "$(INTDIR)\s_sign.obj"
	-@erase "$(INTDIR)\s_sign.sbr"
	-@erase "$(INTDIR)\s_spkey.obj"
	-@erase "$(INTDIR)\s_spkey.sbr"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\s_strver.sbr"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\text2bin.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Sign_wdk.lib"
	-@erase ".\DebugDll\Sign_wdk.bsc"
	-@erase "..\DebugDll\Sign_wdk.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"DebugDll/Sign_wdk.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\errno.sbr" \
	"$(INTDIR)\my_strl.sbr" \
	"$(INTDIR)\n_data.sbr" \
	"$(INTDIR)\s_buflen.sbr" \
	"$(INTDIR)\s_chbuf.sbr" \
	"$(INTDIR)\s_check.sbr" \
	"$(INTDIR)\s_chfile.sbr" \
	"$(INTDIR)\s_chfilw.sbr" \
	"$(INTDIR)\s_chfres.sbr" \
	"$(INTDIR)\s_chres.sbr" \
	"$(INTDIR)\s_cnot.sbr" \
	"$(INTDIR)\s_crpriv.sbr" \
	"$(INTDIR)\s_except.sbr" \
	"$(INTDIR)\s_fnguid.sbr" \
	"$(INTDIR)\s_fnstr.sbr" \
	"$(INTDIR)\s_hash.sbr" \
	"$(INTDIR)\s_klok.sbr" \
	"$(INTDIR)\s_lnfile.sbr" \
	"$(INTDIR)\s_lnot.sbr" \
	"$(INTDIR)\s_putn.sbr" \
	"$(INTDIR)\s_sgnfil.sbr" \
	"$(INTDIR)\s_sgnmem.sbr" \
	"$(INTDIR)\s_sifile.sbr" \
	"$(INTDIR)\s_sign.sbr" \
	"$(INTDIR)\s_spkey.sbr" \
	"$(INTDIR)\s_strver.sbr" \
	"$(INTDIR)\text2bin.sbr"

".\DebugDll\Sign_wdk.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign_wdk.lib" 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_srand.obj" \
	".\objs\c_swdict.obj" \
	".\objs\c_tm.obj" \
	".\objs\c_urand.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\f_sign.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_file.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_gostrf.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_privat.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	"$(INTDIR)\errno.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_chfile.obj" \
	"$(INTDIR)\s_chfilw.obj" \
	"$(INTDIR)\s_chfres.obj" \
	"$(INTDIR)\s_chres.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_crpriv.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnfile.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_putn.obj" \
	"$(INTDIR)\s_sgnfil.obj" \
	"$(INTDIR)\s_sgnmem.obj" \
	"$(INTDIR)\s_sifile.obj" \
	"$(INTDIR)\s_sign.obj" \
	"$(INTDIR)\s_spkey.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign_wdk.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Debug\Sign_wdk.lib
InputName=Sign_wdk
SOURCE="$(InputPath)"

"..\DebugDll\Sign_wdk.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

OUTDIR=.\../../Out/ReleaseS
INTDIR=.\../../Temp/Novell_Release/CommonFiles/Sign
# Begin Custom Macros
OutDir=.\../../Out/ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\Sign_n.lib" "..\Release\Sign_n.lib"


CLEAN :
	-@erase "$(INTDIR)\my_strl.obj"
	-@erase "$(INTDIR)\n_data.obj"
	-@erase "$(INTDIR)\s_buflen.obj"
	-@erase "$(INTDIR)\s_chbuf.obj"
	-@erase "$(INTDIR)\s_check.obj"
	-@erase "$(INTDIR)\s_cnot.obj"
	-@erase "$(INTDIR)\s_except.obj"
	-@erase "$(INTDIR)\s_fnguid.obj"
	-@erase "$(INTDIR)\s_fnstr.obj"
	-@erase "$(INTDIR)\s_hash.obj"
	-@erase "$(INTDIR)\s_klok.obj"
	-@erase "$(INTDIR)\s_lnot.obj"
	-@erase "$(INTDIR)\s_strver.obj"
	-@erase "$(INTDIR)\text2bin.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Sign_n.lib"
	-@erase "..\Release\Sign_n.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /O2 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /D "__WATCOMC__" /D "N_PLAT_NLM" /FA /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\WinSign.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinSign.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\Sign_n.lib" /LINK50COMPAT 
LIB32_OBJS= \
	".\objs\c_coderf.obj" \
	".\objs\c_crc16f.obj" \
	".\objs\c_crc32.obj" \
	".\objs\c_crypto.obj" \
	".\objs\c_decryp.obj" \
	".\objs\c_find.obj" \
	".\objs\c_fs.obj" \
	".\objs\c_hexcod.obj" \
	".\objs\c_mem.obj" \
	".\objs\c_randfi.obj" \
	".\objs\c_randna.obj" \
	".\objs\c_uucode.obj" \
	".\objs\c_verify.obj" \
	".\objs\c_vesta.obj" \
	".\objs\ct_touch.obj" \
	".\objs\ct_ver.obj" \
	".\objs\f_not.obj" \
	".\objs\f_sha.obj" \
	".\objs\n_addkey.obj" \
	".\objs\n_constr.obj" \
	".\objs\n_dsa.obj" \
	".\objs\n_fpub.obj" \
	".\objs\n_hash.obj" \
	".\objs\n_public.obj" \
	".\objs\n_r50.obj" \
	".\objs\n_signrf.obj" \
	".\objs\n_signus.obj" \
	".\objs\n_touch.obj" \
	".\objs\n_vault.obj" \
	".\objs\w_shnor.obj" \
	".\objs\w_signsh.obj" \
	".\objs\cn_urand.obj" \
	".\objs\cn_tm.obj" \
	"$(INTDIR)\my_strl.obj" \
	"$(INTDIR)\n_data.obj" \
	"$(INTDIR)\s_buflen.obj" \
	"$(INTDIR)\s_chbuf.obj" \
	"$(INTDIR)\s_check.obj" \
	"$(INTDIR)\s_cnot.obj" \
	"$(INTDIR)\s_except.obj" \
	"$(INTDIR)\s_fnguid.obj" \
	"$(INTDIR)\s_fnstr.obj" \
	"$(INTDIR)\s_hash.obj" \
	"$(INTDIR)\s_klok.obj" \
	"$(INTDIR)\s_lnot.obj" \
	"$(INTDIR)\s_strver.obj" \
	"$(INTDIR)\text2bin.obj"

"$(OUTDIR)\Sign_n.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseS\Sign_n.lib
InputName=Sign_n
SOURCE="$(InputPath)"

"..\Release\Sign_n.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Release
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
!IF EXISTS("WinSign.dep")
!INCLUDE "WinSign.dep"
!ELSE 
!MESSAGE Warning: cannot find "WinSign.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinSign - Win32 Release" || "$(CFG)" == "WinSign - Win32 Debug" || "$(CFG)" == "WinSign - Win32 DebugDll" || "$(CFG)" == "WinSign - Win32 ReleaseDll" || "$(CFG)" == "WinSign - Win32 Debug With Debug Key" || "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key" || "$(CFG)" == "WinSign - Win32 Novell_Release"
SOURCE=.\src\errno.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\errno.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\errno.obj"	"$(INTDIR)\errno.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\errno.obj"	"$(INTDIR)\errno.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\errno.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\errno.obj"	"$(INTDIR)\errno.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\errno.obj"	"$(INTDIR)\errno.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\my_strl.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\my_strl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\my_strl.obj"	"$(INTDIR)\my_strl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\my_strl.obj"	"$(INTDIR)\my_strl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\my_strl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\my_strl.obj"	"$(INTDIR)\my_strl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\my_strl.obj"	"$(INTDIR)\my_strl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\my_strl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\n_data.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\n_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\n_data.obj"	"$(INTDIR)\n_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\n_data.obj"	"$(INTDIR)\n_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\n_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\n_data.obj"	"$(INTDIR)\n_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\n_data.obj"	"$(INTDIR)\n_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\n_data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_buflen.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_buflen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_buflen.obj"	"$(INTDIR)\s_buflen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_buflen.obj"	"$(INTDIR)\s_buflen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_buflen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_buflen.obj"	"$(INTDIR)\s_buflen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_buflen.obj"	"$(INTDIR)\s_buflen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_buflen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_chbuf.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_chbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_chbuf.obj"	"$(INTDIR)\s_chbuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_chbuf.obj"	"$(INTDIR)\s_chbuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_chbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_chbuf.obj"	"$(INTDIR)\s_chbuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_chbuf.obj"	"$(INTDIR)\s_chbuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_chbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_check.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_check.obj"	"$(INTDIR)\s_check.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_check.obj"	"$(INTDIR)\s_check.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_check.obj"	"$(INTDIR)\s_check.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_check.obj"	"$(INTDIR)\s_check.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_chfile.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_chfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_chfile.obj"	"$(INTDIR)\s_chfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_chfile.obj"	"$(INTDIR)\s_chfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_chfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_chfile.obj"	"$(INTDIR)\s_chfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_chfile.obj"	"$(INTDIR)\s_chfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_chfilw.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_chfilw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_chfilw.obj"	"$(INTDIR)\s_chfilw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_chfilw.obj"	"$(INTDIR)\s_chfilw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_chfilw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_chfilw.obj"	"$(INTDIR)\s_chfilw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_chfilw.obj"	"$(INTDIR)\s_chfilw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_chfres.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_chfres.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_chfres.obj"	"$(INTDIR)\s_chfres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_chfres.obj"	"$(INTDIR)\s_chfres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_chfres.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_chfres.obj"	"$(INTDIR)\s_chfres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_chfres.obj"	"$(INTDIR)\s_chfres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_chres.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_chres.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_chres.obj"	"$(INTDIR)\s_chres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_chres.obj"	"$(INTDIR)\s_chres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_chres.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_chres.obj"	"$(INTDIR)\s_chres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_chres.obj"	"$(INTDIR)\s_chres.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_cnot.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_cnot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_cnot.obj"	"$(INTDIR)\s_cnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_cnot.obj"	"$(INTDIR)\s_cnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_cnot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_cnot.obj"	"$(INTDIR)\s_cnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_cnot.obj"	"$(INTDIR)\s_cnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_cnot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_crpriv.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_crpriv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_crpriv.obj"	"$(INTDIR)\s_crpriv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_crpriv.obj"	"$(INTDIR)\s_crpriv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_crpriv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_crpriv.obj"	"$(INTDIR)\s_crpriv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_crpriv.obj"	"$(INTDIR)\s_crpriv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_except.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_except.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_except.obj"	"$(INTDIR)\s_except.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_except.obj"	"$(INTDIR)\s_except.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_except.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_except.obj"	"$(INTDIR)\s_except.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_except.obj"	"$(INTDIR)\s_except.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_except.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_fnguid.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_fnguid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_fnguid.obj"	"$(INTDIR)\s_fnguid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_fnguid.obj"	"$(INTDIR)\s_fnguid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_fnguid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_fnguid.obj"	"$(INTDIR)\s_fnguid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_fnguid.obj"	"$(INTDIR)\s_fnguid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_fnguid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_fnstr.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_fnstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_fnstr.obj"	"$(INTDIR)\s_fnstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_fnstr.obj"	"$(INTDIR)\s_fnstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_fnstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_fnstr.obj"	"$(INTDIR)\s_fnstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_fnstr.obj"	"$(INTDIR)\s_fnstr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_fnstr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_hash.c

!IF  "$(CFG)" == "WinSign - Win32 Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /GR /GX /O1 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /Fp"$(INTDIR)\WinSign.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\s_hash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\s_hash.obj"	"$(INTDIR)\s_hash.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\s_hash.obj"	"$(INTDIR)\s_hash.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"

CPP_SWITCHES=/nologo /G3 /MD /W3 /GR /GX /O1 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "NO_DEBUG_KEY" /Fp"$(INTDIR)\WinSign.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\s_hash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"

CPP_SWITCHES=/nologo /MTd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\s_hash.obj"	"$(INTDIR)\s_hash.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"

CPP_SWITCHES=/nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I ".." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_DEBUG_KEY" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\s_hash.obj"	"$(INTDIR)\s_hash.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

CPP_SWITCHES=/nologo /G3 /MT /W3 /O1 /I "." /I ".." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "NO_DEBUG_KEY" /D "__WATCOMC__" /D "N_PLAT_NLM" /FA /Fa"$(INTDIR)\\" /Fp"$(INTDIR)\WinSign.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\s_hash.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\s_klok.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_klok.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_klok.obj"	"$(INTDIR)\s_klok.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_klok.obj"	"$(INTDIR)\s_klok.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_klok.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_klok.obj"	"$(INTDIR)\s_klok.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_klok.obj"	"$(INTDIR)\s_klok.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_klok.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_lnfile.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_lnfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_lnfile.obj"	"$(INTDIR)\s_lnfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_lnfile.obj"	"$(INTDIR)\s_lnfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_lnfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_lnfile.obj"	"$(INTDIR)\s_lnfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_lnfile.obj"	"$(INTDIR)\s_lnfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_lnot.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_lnot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_lnot.obj"	"$(INTDIR)\s_lnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_lnot.obj"	"$(INTDIR)\s_lnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_lnot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_lnot.obj"	"$(INTDIR)\s_lnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_lnot.obj"	"$(INTDIR)\s_lnot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_lnot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\s_putn.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_putn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_putn.obj"	"$(INTDIR)\s_putn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_putn.obj"	"$(INTDIR)\s_putn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_putn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_putn.obj"	"$(INTDIR)\s_putn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_putn.obj"	"$(INTDIR)\s_putn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_sgnfil.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_sgnfil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_sgnfil.obj"	"$(INTDIR)\s_sgnfil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_sgnfil.obj"	"$(INTDIR)\s_sgnfil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_sgnfil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_sgnfil.obj"	"$(INTDIR)\s_sgnfil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_sgnfil.obj"	"$(INTDIR)\s_sgnfil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_sgnmem.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_sgnmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_sgnmem.obj"	"$(INTDIR)\s_sgnmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_sgnmem.obj"	"$(INTDIR)\s_sgnmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_sgnmem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_sgnmem.obj"	"$(INTDIR)\s_sgnmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_sgnmem.obj"	"$(INTDIR)\s_sgnmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_sifile.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_sifile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_sifile.obj"	"$(INTDIR)\s_sifile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_sifile.obj"	"$(INTDIR)\s_sifile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_sifile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_sifile.obj"	"$(INTDIR)\s_sifile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_sifile.obj"	"$(INTDIR)\s_sifile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_sign.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_sign.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_sign.obj"	"$(INTDIR)\s_sign.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_sign.obj"	"$(INTDIR)\s_sign.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_sign.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_sign.obj"	"$(INTDIR)\s_sign.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_sign.obj"	"$(INTDIR)\s_sign.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_spkey.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_spkey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_spkey.obj"	"$(INTDIR)\s_spkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_spkey.obj"	"$(INTDIR)\s_spkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_spkey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_spkey.obj"	"$(INTDIR)\s_spkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_spkey.obj"	"$(INTDIR)\s_spkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"

!ENDIF 

SOURCE=.\src\s_strver.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\s_strver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\s_strver.obj"	"$(INTDIR)\s_strver.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\s_strver.obj"	"$(INTDIR)\s_strver.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\s_strver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\s_strver.obj"	"$(INTDIR)\s_strver.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\s_strver.obj"	"$(INTDIR)\s_strver.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\s_strver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\stuff\text2bin.c

!IF  "$(CFG)" == "WinSign - Win32 Release"


"$(INTDIR)\text2bin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug"


"$(INTDIR)\text2bin.obj"	"$(INTDIR)\text2bin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll"


"$(INTDIR)\text2bin.obj"	"$(INTDIR)\text2bin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 ReleaseDll"


"$(INTDIR)\text2bin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Debug With Debug Key"


"$(INTDIR)\text2bin.obj"	"$(INTDIR)\text2bin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 DebugDll With Debug Key"


"$(INTDIR)\text2bin.obj"	"$(INTDIR)\text2bin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "WinSign - Win32 Novell_Release"


"$(INTDIR)\text2bin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on Property.dsp
!IF "$(CFG)" == ""
CFG=Property - Win32 Unicode Debug
!MESSAGE No configuration specified. Defaulting to Property - Win32 Unicode Debug.
!ENDIF 

!IF "$(CFG)" != "Property - Win32 Release" && "$(CFG)" != "Property - Win32 Debug" && "$(CFG)" != "Property - Win32 DebugDll" && "$(CFG)" != "Property - Win32 ReleaseDll" && "$(CFG)" != "Property - Win32 Unicode DebugDll" && "$(CFG)" != "Property - Win32 Unicode Release" && "$(CFG)" != "Property - Win32 Unicode Debug" && "$(CFG)" != "Property - Win32 Unicode ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Property.mak" CFG="Property - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Property - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Property - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "Property - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\CommonFiles\Release\Property.lib"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\Release\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Release\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\Release\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\CommonFiles\Debug\Property.lib" "$(OUTDIR)\Property.bsc"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\CRC.SBR"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add.sbr"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_add_pr.sbr"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_attach.sbr"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_comp.sbr"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_copy.sbr"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_deser.sbr"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_detach.sbr"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_each.sbr"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_f_that.sbr"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find.sbr"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_find_p.sbr"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_free.sbr"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_get.sbr"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_getval.sbr"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_init.sbr"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_insert.sbr"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_merge.sbr"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_mult.sbr"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_remove.sbr"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_rempr.sbr"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_repl_i.sbr"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replce.sbr"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_replpr.sbr"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_ser.sbr"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\d_setval.sbr"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_arr.sbr"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_catch.sbr"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_copy.sbr"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_free.sbr"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_get.sbr"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_init.sbr"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_mult.sbr"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_predct.sbr"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\p_replce.sbr"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_gitms.sbr"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_insrt.sbr"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_remve.sbr"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\pa_sitms.sbr"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\prop_wrp.sbr"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec.sbr"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_add.sbr"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_allc.sbr"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_comp.sbr"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\sec_make.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Property.bsc"
	-@erase "..\CommonFiles\Debug\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CRC.SBR" \
	"$(INTDIR)\d_add.sbr" \
	"$(INTDIR)\d_add_pr.sbr" \
	"$(INTDIR)\d_attach.sbr" \
	"$(INTDIR)\d_comp.sbr" \
	"$(INTDIR)\d_copy.sbr" \
	"$(INTDIR)\d_deser.sbr" \
	"$(INTDIR)\d_detach.sbr" \
	"$(INTDIR)\d_f_each.sbr" \
	"$(INTDIR)\d_f_that.sbr" \
	"$(INTDIR)\d_find.sbr" \
	"$(INTDIR)\d_find_p.sbr" \
	"$(INTDIR)\d_free.sbr" \
	"$(INTDIR)\d_get.sbr" \
	"$(INTDIR)\d_getval.sbr" \
	"$(INTDIR)\d_init.sbr" \
	"$(INTDIR)\d_insert.sbr" \
	"$(INTDIR)\d_merge.sbr" \
	"$(INTDIR)\d_mult.sbr" \
	"$(INTDIR)\d_remove.sbr" \
	"$(INTDIR)\d_rempr.sbr" \
	"$(INTDIR)\d_repl_i.sbr" \
	"$(INTDIR)\d_replce.sbr" \
	"$(INTDIR)\d_replpr.sbr" \
	"$(INTDIR)\d_ser.sbr" \
	"$(INTDIR)\d_setval.sbr" \
	"$(INTDIR)\p_arr.sbr" \
	"$(INTDIR)\p_catch.sbr" \
	"$(INTDIR)\p_copy.sbr" \
	"$(INTDIR)\p_free.sbr" \
	"$(INTDIR)\p_get.sbr" \
	"$(INTDIR)\p_init.sbr" \
	"$(INTDIR)\p_mult.sbr" \
	"$(INTDIR)\p_predct.sbr" \
	"$(INTDIR)\p_replce.sbr" \
	"$(INTDIR)\pa_gitms.sbr" \
	"$(INTDIR)\pa_insrt.sbr" \
	"$(INTDIR)\pa_remve.sbr" \
	"$(INTDIR)\pa_sitms.sbr" \
	"$(INTDIR)\prop_wrp.sbr" \
	"$(INTDIR)\sec.sbr" \
	"$(INTDIR)\sec_add.sbr" \
	"$(INTDIR)\sec_allc.sbr" \
	"$(INTDIR)\sec_comp.sbr" \
	"$(INTDIR)\sec_make.sbr"

"$(OUTDIR)\Property.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Debug\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\Debug\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"

OUTDIR=.\DebugDll
INTDIR=.\DebugDll
# Begin Custom Macros
OutDir=.\DebugDll
# End Custom Macros

ALL : "..\CommonFiles\DebugDll\Property.lib" "$(OUTDIR)\Property.bsc"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\CRC.SBR"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add.sbr"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_add_pr.sbr"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_attach.sbr"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_comp.sbr"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_copy.sbr"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_deser.sbr"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_detach.sbr"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_each.sbr"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_f_that.sbr"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find.sbr"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_find_p.sbr"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_free.sbr"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_get.sbr"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_getval.sbr"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_init.sbr"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_insert.sbr"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_merge.sbr"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_mult.sbr"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_remove.sbr"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_rempr.sbr"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_repl_i.sbr"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replce.sbr"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_replpr.sbr"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_ser.sbr"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\d_setval.sbr"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_arr.sbr"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_catch.sbr"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_copy.sbr"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_free.sbr"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_get.sbr"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_init.sbr"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_mult.sbr"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_predct.sbr"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\p_replce.sbr"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_gitms.sbr"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_insrt.sbr"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_remve.sbr"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\pa_sitms.sbr"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\prop_wrp.sbr"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec.sbr"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_add.sbr"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_allc.sbr"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_comp.sbr"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\sec_make.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Property.bsc"
	-@erase "..\CommonFiles\DebugDll\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CRC.SBR" \
	"$(INTDIR)\d_add.sbr" \
	"$(INTDIR)\d_add_pr.sbr" \
	"$(INTDIR)\d_attach.sbr" \
	"$(INTDIR)\d_comp.sbr" \
	"$(INTDIR)\d_copy.sbr" \
	"$(INTDIR)\d_deser.sbr" \
	"$(INTDIR)\d_detach.sbr" \
	"$(INTDIR)\d_f_each.sbr" \
	"$(INTDIR)\d_f_that.sbr" \
	"$(INTDIR)\d_find.sbr" \
	"$(INTDIR)\d_find_p.sbr" \
	"$(INTDIR)\d_free.sbr" \
	"$(INTDIR)\d_get.sbr" \
	"$(INTDIR)\d_getval.sbr" \
	"$(INTDIR)\d_init.sbr" \
	"$(INTDIR)\d_insert.sbr" \
	"$(INTDIR)\d_merge.sbr" \
	"$(INTDIR)\d_mult.sbr" \
	"$(INTDIR)\d_remove.sbr" \
	"$(INTDIR)\d_rempr.sbr" \
	"$(INTDIR)\d_repl_i.sbr" \
	"$(INTDIR)\d_replce.sbr" \
	"$(INTDIR)\d_replpr.sbr" \
	"$(INTDIR)\d_ser.sbr" \
	"$(INTDIR)\d_setval.sbr" \
	"$(INTDIR)\p_arr.sbr" \
	"$(INTDIR)\p_catch.sbr" \
	"$(INTDIR)\p_copy.sbr" \
	"$(INTDIR)\p_free.sbr" \
	"$(INTDIR)\p_get.sbr" \
	"$(INTDIR)\p_init.sbr" \
	"$(INTDIR)\p_mult.sbr" \
	"$(INTDIR)\p_predct.sbr" \
	"$(INTDIR)\p_replce.sbr" \
	"$(INTDIR)\pa_gitms.sbr" \
	"$(INTDIR)\pa_insrt.sbr" \
	"$(INTDIR)\pa_remve.sbr" \
	"$(INTDIR)\pa_sitms.sbr" \
	"$(INTDIR)\prop_wrp.sbr" \
	"$(INTDIR)\sec.sbr" \
	"$(INTDIR)\sec_add.sbr" \
	"$(INTDIR)\sec_allc.sbr" \
	"$(INTDIR)\sec_comp.sbr" \
	"$(INTDIR)\sec_make.sbr"

"$(OUTDIR)\Property.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugDll\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\DebugDll\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"

OUTDIR=.\ReleaseDll
INTDIR=.\ReleaseDll

ALL : "..\CommonFiles\ReleaseDll\Property.lib"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseDll\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseDll\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\ReleaseDll\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"

OUTDIR=.\DebugUDll
INTDIR=.\DebugUDll
# Begin Custom Macros
OutDir=.\DebugUDll
# End Custom Macros

ALL : "..\CommonFiles\DebugUDll\Property.lib" "$(OUTDIR)\Property.bsc"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\CRC.SBR"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add.sbr"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_add_pr.sbr"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_attach.sbr"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_comp.sbr"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_copy.sbr"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_deser.sbr"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_detach.sbr"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_each.sbr"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_f_that.sbr"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find.sbr"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_find_p.sbr"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_free.sbr"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_get.sbr"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_getval.sbr"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_init.sbr"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_insert.sbr"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_merge.sbr"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_mult.sbr"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_remove.sbr"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_rempr.sbr"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_repl_i.sbr"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replce.sbr"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_replpr.sbr"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_ser.sbr"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\d_setval.sbr"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_arr.sbr"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_catch.sbr"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_copy.sbr"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_free.sbr"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_get.sbr"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_init.sbr"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_mult.sbr"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_predct.sbr"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\p_replce.sbr"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_gitms.sbr"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_insrt.sbr"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_remve.sbr"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\pa_sitms.sbr"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\prop_wrp.sbr"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec.sbr"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_add.sbr"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_allc.sbr"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_comp.sbr"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\sec_make.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Property.bsc"
	-@erase "..\CommonFiles\DebugUDll\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CRC.SBR" \
	"$(INTDIR)\d_add.sbr" \
	"$(INTDIR)\d_add_pr.sbr" \
	"$(INTDIR)\d_attach.sbr" \
	"$(INTDIR)\d_comp.sbr" \
	"$(INTDIR)\d_copy.sbr" \
	"$(INTDIR)\d_deser.sbr" \
	"$(INTDIR)\d_detach.sbr" \
	"$(INTDIR)\d_f_each.sbr" \
	"$(INTDIR)\d_f_that.sbr" \
	"$(INTDIR)\d_find.sbr" \
	"$(INTDIR)\d_find_p.sbr" \
	"$(INTDIR)\d_free.sbr" \
	"$(INTDIR)\d_get.sbr" \
	"$(INTDIR)\d_getval.sbr" \
	"$(INTDIR)\d_init.sbr" \
	"$(INTDIR)\d_insert.sbr" \
	"$(INTDIR)\d_merge.sbr" \
	"$(INTDIR)\d_mult.sbr" \
	"$(INTDIR)\d_remove.sbr" \
	"$(INTDIR)\d_rempr.sbr" \
	"$(INTDIR)\d_repl_i.sbr" \
	"$(INTDIR)\d_replce.sbr" \
	"$(INTDIR)\d_replpr.sbr" \
	"$(INTDIR)\d_ser.sbr" \
	"$(INTDIR)\d_setval.sbr" \
	"$(INTDIR)\p_arr.sbr" \
	"$(INTDIR)\p_catch.sbr" \
	"$(INTDIR)\p_copy.sbr" \
	"$(INTDIR)\p_free.sbr" \
	"$(INTDIR)\p_get.sbr" \
	"$(INTDIR)\p_init.sbr" \
	"$(INTDIR)\p_mult.sbr" \
	"$(INTDIR)\p_predct.sbr" \
	"$(INTDIR)\p_replce.sbr" \
	"$(INTDIR)\pa_gitms.sbr" \
	"$(INTDIR)\pa_insrt.sbr" \
	"$(INTDIR)\pa_remve.sbr" \
	"$(INTDIR)\pa_sitms.sbr" \
	"$(INTDIR)\prop_wrp.sbr" \
	"$(INTDIR)\sec.sbr" \
	"$(INTDIR)\sec_add.sbr" \
	"$(INTDIR)\sec_allc.sbr" \
	"$(INTDIR)\sec_comp.sbr" \
	"$(INTDIR)\sec_make.sbr"

"$(OUTDIR)\Property.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugUDll\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\DebugUDll\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"

OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : "..\CommonFiles\ReleaseU\Property.lib"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseU\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseU\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\ReleaseU\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

ALL : "..\CommonFiles\DebugU\Property.lib" "$(OUTDIR)\Property.bsc"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\CRC.SBR"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add.sbr"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_add_pr.sbr"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_attach.sbr"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_comp.sbr"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_copy.sbr"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_deser.sbr"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_detach.sbr"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_each.sbr"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_f_that.sbr"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find.sbr"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_find_p.sbr"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_free.sbr"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_get.sbr"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_getval.sbr"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_init.sbr"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_insert.sbr"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_merge.sbr"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_mult.sbr"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_remove.sbr"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_rempr.sbr"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_repl_i.sbr"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replce.sbr"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_replpr.sbr"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_ser.sbr"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\d_setval.sbr"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_arr.sbr"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_catch.sbr"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_copy.sbr"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_free.sbr"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_get.sbr"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_init.sbr"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_mult.sbr"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_predct.sbr"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\p_replce.sbr"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_gitms.sbr"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_insrt.sbr"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_remve.sbr"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\pa_sitms.sbr"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\prop_wrp.sbr"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec.sbr"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_add.sbr"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_allc.sbr"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_comp.sbr"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\sec_make.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Property.bsc"
	-@erase "..\CommonFiles\DebugU\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "_DEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CRC.SBR" \
	"$(INTDIR)\d_add.sbr" \
	"$(INTDIR)\d_add_pr.sbr" \
	"$(INTDIR)\d_attach.sbr" \
	"$(INTDIR)\d_comp.sbr" \
	"$(INTDIR)\d_copy.sbr" \
	"$(INTDIR)\d_deser.sbr" \
	"$(INTDIR)\d_detach.sbr" \
	"$(INTDIR)\d_f_each.sbr" \
	"$(INTDIR)\d_f_that.sbr" \
	"$(INTDIR)\d_find.sbr" \
	"$(INTDIR)\d_find_p.sbr" \
	"$(INTDIR)\d_free.sbr" \
	"$(INTDIR)\d_get.sbr" \
	"$(INTDIR)\d_getval.sbr" \
	"$(INTDIR)\d_init.sbr" \
	"$(INTDIR)\d_insert.sbr" \
	"$(INTDIR)\d_merge.sbr" \
	"$(INTDIR)\d_mult.sbr" \
	"$(INTDIR)\d_remove.sbr" \
	"$(INTDIR)\d_rempr.sbr" \
	"$(INTDIR)\d_repl_i.sbr" \
	"$(INTDIR)\d_replce.sbr" \
	"$(INTDIR)\d_replpr.sbr" \
	"$(INTDIR)\d_ser.sbr" \
	"$(INTDIR)\d_setval.sbr" \
	"$(INTDIR)\p_arr.sbr" \
	"$(INTDIR)\p_catch.sbr" \
	"$(INTDIR)\p_copy.sbr" \
	"$(INTDIR)\p_free.sbr" \
	"$(INTDIR)\p_get.sbr" \
	"$(INTDIR)\p_init.sbr" \
	"$(INTDIR)\p_mult.sbr" \
	"$(INTDIR)\p_predct.sbr" \
	"$(INTDIR)\p_replce.sbr" \
	"$(INTDIR)\pa_gitms.sbr" \
	"$(INTDIR)\pa_insrt.sbr" \
	"$(INTDIR)\pa_remve.sbr" \
	"$(INTDIR)\pa_sitms.sbr" \
	"$(INTDIR)\prop_wrp.sbr" \
	"$(INTDIR)\sec.sbr" \
	"$(INTDIR)\sec_add.sbr" \
	"$(INTDIR)\sec_allc.sbr" \
	"$(INTDIR)\sec_comp.sbr" \
	"$(INTDIR)\sec_make.sbr"

"$(OUTDIR)\Property.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugU\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\DebugU\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"

OUTDIR=.\ReleaseUDll
INTDIR=.\ReleaseUDll

ALL : "..\CommonFiles\ReleaseUDll\Property.lib"


CLEAN :
	-@erase "$(INTDIR)\CRC.OBJ"
	-@erase "$(INTDIR)\d_add.obj"
	-@erase "$(INTDIR)\d_add_pr.obj"
	-@erase "$(INTDIR)\d_attach.obj"
	-@erase "$(INTDIR)\d_comp.obj"
	-@erase "$(INTDIR)\d_copy.obj"
	-@erase "$(INTDIR)\d_deser.obj"
	-@erase "$(INTDIR)\d_detach.obj"
	-@erase "$(INTDIR)\d_f_each.obj"
	-@erase "$(INTDIR)\d_f_that.obj"
	-@erase "$(INTDIR)\d_find.obj"
	-@erase "$(INTDIR)\d_find_p.obj"
	-@erase "$(INTDIR)\d_free.obj"
	-@erase "$(INTDIR)\d_get.obj"
	-@erase "$(INTDIR)\d_getval.obj"
	-@erase "$(INTDIR)\d_init.obj"
	-@erase "$(INTDIR)\d_insert.obj"
	-@erase "$(INTDIR)\d_merge.obj"
	-@erase "$(INTDIR)\d_mult.obj"
	-@erase "$(INTDIR)\d_remove.obj"
	-@erase "$(INTDIR)\d_rempr.obj"
	-@erase "$(INTDIR)\d_repl_i.obj"
	-@erase "$(INTDIR)\d_replce.obj"
	-@erase "$(INTDIR)\d_replpr.obj"
	-@erase "$(INTDIR)\d_ser.obj"
	-@erase "$(INTDIR)\d_setval.obj"
	-@erase "$(INTDIR)\p_arr.obj"
	-@erase "$(INTDIR)\p_catch.obj"
	-@erase "$(INTDIR)\p_copy.obj"
	-@erase "$(INTDIR)\p_free.obj"
	-@erase "$(INTDIR)\p_get.obj"
	-@erase "$(INTDIR)\p_init.obj"
	-@erase "$(INTDIR)\p_mult.obj"
	-@erase "$(INTDIR)\p_predct.obj"
	-@erase "$(INTDIR)\p_replce.obj"
	-@erase "$(INTDIR)\pa_gitms.obj"
	-@erase "$(INTDIR)\pa_insrt.obj"
	-@erase "$(INTDIR)\pa_remve.obj"
	-@erase "$(INTDIR)\pa_sitms.obj"
	-@erase "$(INTDIR)\prop_wrp.obj"
	-@erase "$(INTDIR)\sec.obj"
	-@erase "$(INTDIR)\sec_add.obj"
	-@erase "$(INTDIR)\sec_allc.obj"
	-@erase "$(INTDIR)\sec_comp.obj"
	-@erase "$(INTDIR)\sec_make.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseUDll\Property.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "NDEBUG" /D "WIN32" /D "UNICODE" /D "_LIB" /D "M_UTILS" /Fp"$(INTDIR)\Property.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Property.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseUDll\Property.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CRC.OBJ" \
	"$(INTDIR)\d_add.obj" \
	"$(INTDIR)\d_add_pr.obj" \
	"$(INTDIR)\d_attach.obj" \
	"$(INTDIR)\d_comp.obj" \
	"$(INTDIR)\d_copy.obj" \
	"$(INTDIR)\d_deser.obj" \
	"$(INTDIR)\d_detach.obj" \
	"$(INTDIR)\d_f_each.obj" \
	"$(INTDIR)\d_f_that.obj" \
	"$(INTDIR)\d_find.obj" \
	"$(INTDIR)\d_find_p.obj" \
	"$(INTDIR)\d_free.obj" \
	"$(INTDIR)\d_get.obj" \
	"$(INTDIR)\d_getval.obj" \
	"$(INTDIR)\d_init.obj" \
	"$(INTDIR)\d_insert.obj" \
	"$(INTDIR)\d_merge.obj" \
	"$(INTDIR)\d_mult.obj" \
	"$(INTDIR)\d_remove.obj" \
	"$(INTDIR)\d_rempr.obj" \
	"$(INTDIR)\d_repl_i.obj" \
	"$(INTDIR)\d_replce.obj" \
	"$(INTDIR)\d_replpr.obj" \
	"$(INTDIR)\d_ser.obj" \
	"$(INTDIR)\d_setval.obj" \
	"$(INTDIR)\p_arr.obj" \
	"$(INTDIR)\p_catch.obj" \
	"$(INTDIR)\p_copy.obj" \
	"$(INTDIR)\p_free.obj" \
	"$(INTDIR)\p_get.obj" \
	"$(INTDIR)\p_init.obj" \
	"$(INTDIR)\p_mult.obj" \
	"$(INTDIR)\p_predct.obj" \
	"$(INTDIR)\p_replce.obj" \
	"$(INTDIR)\pa_gitms.obj" \
	"$(INTDIR)\pa_insrt.obj" \
	"$(INTDIR)\pa_remve.obj" \
	"$(INTDIR)\pa_sitms.obj" \
	"$(INTDIR)\prop_wrp.obj" \
	"$(INTDIR)\sec.obj" \
	"$(INTDIR)\sec_add.obj" \
	"$(INTDIR)\sec_allc.obj" \
	"$(INTDIR)\sec_comp.obj" \
	"$(INTDIR)\sec_make.obj"

"..\CommonFiles\ReleaseUDll\Property.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("Property.dep")
!INCLUDE "Property.dep"
!ELSE 
!MESSAGE Warning: cannot find "Property.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Property - Win32 Release" || "$(CFG)" == "Property - Win32 Debug" || "$(CFG)" == "Property - Win32 DebugDll" || "$(CFG)" == "Property - Win32 ReleaseDll" || "$(CFG)" == "Property - Win32 Unicode DebugDll" || "$(CFG)" == "Property - Win32 Unicode Release" || "$(CFG)" == "Property - Win32 Unicode Debug" || "$(CFG)" == "Property - Win32 Unicode ReleaseDll"
SOURCE=.\CRC.C

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\CRC.OBJ" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\CRC.OBJ"	"$(INTDIR)\CRC.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\CRC.OBJ"	"$(INTDIR)\CRC.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\CRC.OBJ" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\CRC.OBJ"	"$(INTDIR)\CRC.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\CRC.OBJ" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\CRC.OBJ"	"$(INTDIR)\CRC.SBR" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\CRC.OBJ" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_add.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_add.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_add.obj"	"$(INTDIR)\d_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_add.obj"	"$(INTDIR)\d_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_add.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_add.obj"	"$(INTDIR)\d_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_add.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_add.obj"	"$(INTDIR)\d_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_add.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_add_pr.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_add_pr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_add_pr.obj"	"$(INTDIR)\d_add_pr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_add_pr.obj"	"$(INTDIR)\d_add_pr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_add_pr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_add_pr.obj"	"$(INTDIR)\d_add_pr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_add_pr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_add_pr.obj"	"$(INTDIR)\d_add_pr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_add_pr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_attach.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_attach.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_attach.obj"	"$(INTDIR)\d_attach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_attach.obj"	"$(INTDIR)\d_attach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_attach.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_attach.obj"	"$(INTDIR)\d_attach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_attach.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_attach.obj"	"$(INTDIR)\d_attach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_attach.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_comp.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_comp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_comp.obj"	"$(INTDIR)\d_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_comp.obj"	"$(INTDIR)\d_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_comp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_comp.obj"	"$(INTDIR)\d_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_comp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_comp.obj"	"$(INTDIR)\d_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_comp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_copy.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_copy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_copy.obj"	"$(INTDIR)\d_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_copy.obj"	"$(INTDIR)\d_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_copy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_copy.obj"	"$(INTDIR)\d_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_copy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_copy.obj"	"$(INTDIR)\d_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_copy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_deser.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_deser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_deser.obj"	"$(INTDIR)\d_deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_deser.obj"	"$(INTDIR)\d_deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_deser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_deser.obj"	"$(INTDIR)\d_deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_deser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_deser.obj"	"$(INTDIR)\d_deser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_deser.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_detach.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_detach.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_detach.obj"	"$(INTDIR)\d_detach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_detach.obj"	"$(INTDIR)\d_detach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_detach.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_detach.obj"	"$(INTDIR)\d_detach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_detach.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_detach.obj"	"$(INTDIR)\d_detach.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_detach.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_f_each.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_f_each.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_f_each.obj"	"$(INTDIR)\d_f_each.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_f_each.obj"	"$(INTDIR)\d_f_each.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_f_each.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_f_each.obj"	"$(INTDIR)\d_f_each.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_f_each.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_f_each.obj"	"$(INTDIR)\d_f_each.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_f_each.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_f_that.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_f_that.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_f_that.obj"	"$(INTDIR)\d_f_that.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_f_that.obj"	"$(INTDIR)\d_f_that.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_f_that.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_f_that.obj"	"$(INTDIR)\d_f_that.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_f_that.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_f_that.obj"	"$(INTDIR)\d_f_that.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_f_that.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_find.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_find.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_find.obj"	"$(INTDIR)\d_find.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_find.obj"	"$(INTDIR)\d_find.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_find.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_find.obj"	"$(INTDIR)\d_find.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_find.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_find.obj"	"$(INTDIR)\d_find.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_find.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_find_p.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_find_p.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_find_p.obj"	"$(INTDIR)\d_find_p.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_find_p.obj"	"$(INTDIR)\d_find_p.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_find_p.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_find_p.obj"	"$(INTDIR)\d_find_p.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_find_p.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_find_p.obj"	"$(INTDIR)\d_find_p.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_find_p.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_free.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_free.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_free.obj"	"$(INTDIR)\d_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_free.obj"	"$(INTDIR)\d_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_free.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_free.obj"	"$(INTDIR)\d_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_free.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_free.obj"	"$(INTDIR)\d_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_free.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_get.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_get.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_get.obj"	"$(INTDIR)\d_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_get.obj"	"$(INTDIR)\d_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_get.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_get.obj"	"$(INTDIR)\d_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_get.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_get.obj"	"$(INTDIR)\d_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_get.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_getval.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_getval.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_getval.obj"	"$(INTDIR)\d_getval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_getval.obj"	"$(INTDIR)\d_getval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_getval.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_getval.obj"	"$(INTDIR)\d_getval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_getval.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_getval.obj"	"$(INTDIR)\d_getval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_getval.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_init.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_init.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_init.obj"	"$(INTDIR)\d_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_init.obj"	"$(INTDIR)\d_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_init.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_init.obj"	"$(INTDIR)\d_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_init.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_init.obj"	"$(INTDIR)\d_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_init.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_insert.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_insert.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_insert.obj"	"$(INTDIR)\d_insert.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_insert.obj"	"$(INTDIR)\d_insert.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_insert.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_insert.obj"	"$(INTDIR)\d_insert.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_insert.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_insert.obj"	"$(INTDIR)\d_insert.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_insert.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_merge.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_merge.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_merge.obj"	"$(INTDIR)\d_merge.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_merge.obj"	"$(INTDIR)\d_merge.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_merge.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_merge.obj"	"$(INTDIR)\d_merge.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_merge.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_merge.obj"	"$(INTDIR)\d_merge.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_merge.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_mult.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_mult.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_mult.obj"	"$(INTDIR)\d_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_mult.obj"	"$(INTDIR)\d_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_mult.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_mult.obj"	"$(INTDIR)\d_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_mult.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_mult.obj"	"$(INTDIR)\d_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_mult.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_remove.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_remove.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_remove.obj"	"$(INTDIR)\d_remove.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_remove.obj"	"$(INTDIR)\d_remove.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_remove.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_remove.obj"	"$(INTDIR)\d_remove.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_remove.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_remove.obj"	"$(INTDIR)\d_remove.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_remove.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_rempr.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_rempr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_rempr.obj"	"$(INTDIR)\d_rempr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_rempr.obj"	"$(INTDIR)\d_rempr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_rempr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_rempr.obj"	"$(INTDIR)\d_rempr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_rempr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_rempr.obj"	"$(INTDIR)\d_rempr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_rempr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_repl_i.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_repl_i.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_repl_i.obj"	"$(INTDIR)\d_repl_i.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_repl_i.obj"	"$(INTDIR)\d_repl_i.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_repl_i.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_repl_i.obj"	"$(INTDIR)\d_repl_i.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_repl_i.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_repl_i.obj"	"$(INTDIR)\d_repl_i.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_repl_i.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_replce.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_replce.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_replce.obj"	"$(INTDIR)\d_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_replce.obj"	"$(INTDIR)\d_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_replce.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_replce.obj"	"$(INTDIR)\d_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_replce.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_replce.obj"	"$(INTDIR)\d_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_replce.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_replpr.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_replpr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_replpr.obj"	"$(INTDIR)\d_replpr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_replpr.obj"	"$(INTDIR)\d_replpr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_replpr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_replpr.obj"	"$(INTDIR)\d_replpr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_replpr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_replpr.obj"	"$(INTDIR)\d_replpr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_replpr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_ser.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_ser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_ser.obj"	"$(INTDIR)\d_ser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_ser.obj"	"$(INTDIR)\d_ser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_ser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_ser.obj"	"$(INTDIR)\d_ser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_ser.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_ser.obj"	"$(INTDIR)\d_ser.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_ser.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\d_setval.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\d_setval.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\d_setval.obj"	"$(INTDIR)\d_setval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\d_setval.obj"	"$(INTDIR)\d_setval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\d_setval.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\d_setval.obj"	"$(INTDIR)\d_setval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\d_setval.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\d_setval.obj"	"$(INTDIR)\d_setval.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\d_setval.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_arr.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_arr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_arr.obj"	"$(INTDIR)\p_arr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_arr.obj"	"$(INTDIR)\p_arr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_arr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_arr.obj"	"$(INTDIR)\p_arr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_arr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_arr.obj"	"$(INTDIR)\p_arr.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_arr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_catch.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_catch.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_catch.obj"	"$(INTDIR)\p_catch.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_catch.obj"	"$(INTDIR)\p_catch.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_catch.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_catch.obj"	"$(INTDIR)\p_catch.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_catch.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_catch.obj"	"$(INTDIR)\p_catch.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_catch.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_copy.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_copy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_copy.obj"	"$(INTDIR)\p_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_copy.obj"	"$(INTDIR)\p_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_copy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_copy.obj"	"$(INTDIR)\p_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_copy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_copy.obj"	"$(INTDIR)\p_copy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_copy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_free.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_free.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_free.obj"	"$(INTDIR)\p_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_free.obj"	"$(INTDIR)\p_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_free.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_free.obj"	"$(INTDIR)\p_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_free.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_free.obj"	"$(INTDIR)\p_free.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_free.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_get.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_get.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_get.obj"	"$(INTDIR)\p_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_get.obj"	"$(INTDIR)\p_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_get.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_get.obj"	"$(INTDIR)\p_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_get.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_get.obj"	"$(INTDIR)\p_get.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_get.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_init.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_init.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_init.obj"	"$(INTDIR)\p_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_init.obj"	"$(INTDIR)\p_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_init.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_init.obj"	"$(INTDIR)\p_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_init.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_init.obj"	"$(INTDIR)\p_init.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_init.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_mult.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_mult.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_mult.obj"	"$(INTDIR)\p_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_mult.obj"	"$(INTDIR)\p_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_mult.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_mult.obj"	"$(INTDIR)\p_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_mult.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_mult.obj"	"$(INTDIR)\p_mult.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_mult.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_predct.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_predct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_predct.obj"	"$(INTDIR)\p_predct.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_predct.obj"	"$(INTDIR)\p_predct.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_predct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_predct.obj"	"$(INTDIR)\p_predct.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_predct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_predct.obj"	"$(INTDIR)\p_predct.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_predct.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\p_replce.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\p_replce.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\p_replce.obj"	"$(INTDIR)\p_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\p_replce.obj"	"$(INTDIR)\p_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\p_replce.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\p_replce.obj"	"$(INTDIR)\p_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\p_replce.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\p_replce.obj"	"$(INTDIR)\p_replce.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\p_replce.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pa_gitms.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\pa_gitms.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\pa_gitms.obj"	"$(INTDIR)\pa_gitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\pa_gitms.obj"	"$(INTDIR)\pa_gitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\pa_gitms.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\pa_gitms.obj"	"$(INTDIR)\pa_gitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\pa_gitms.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\pa_gitms.obj"	"$(INTDIR)\pa_gitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\pa_gitms.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pa_insrt.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\pa_insrt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\pa_insrt.obj"	"$(INTDIR)\pa_insrt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\pa_insrt.obj"	"$(INTDIR)\pa_insrt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\pa_insrt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\pa_insrt.obj"	"$(INTDIR)\pa_insrt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\pa_insrt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\pa_insrt.obj"	"$(INTDIR)\pa_insrt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\pa_insrt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pa_remve.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\pa_remve.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\pa_remve.obj"	"$(INTDIR)\pa_remve.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\pa_remve.obj"	"$(INTDIR)\pa_remve.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\pa_remve.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\pa_remve.obj"	"$(INTDIR)\pa_remve.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\pa_remve.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\pa_remve.obj"	"$(INTDIR)\pa_remve.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\pa_remve.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pa_sitms.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\pa_sitms.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\pa_sitms.obj"	"$(INTDIR)\pa_sitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\pa_sitms.obj"	"$(INTDIR)\pa_sitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\pa_sitms.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\pa_sitms.obj"	"$(INTDIR)\pa_sitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\pa_sitms.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\pa_sitms.obj"	"$(INTDIR)\pa_sitms.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\pa_sitms.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\prop_wrp.cpp

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\prop_wrp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\prop_wrp.obj"	"$(INTDIR)\prop_wrp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\prop_wrp.obj"	"$(INTDIR)\prop_wrp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\prop_wrp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\prop_wrp.obj"	"$(INTDIR)\prop_wrp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\prop_wrp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\prop_wrp.obj"	"$(INTDIR)\prop_wrp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\prop_wrp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sec.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\sec.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\sec.obj"	"$(INTDIR)\sec.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\sec.obj"	"$(INTDIR)\sec.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\sec.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\sec.obj"	"$(INTDIR)\sec.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\sec.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\sec.obj"	"$(INTDIR)\sec.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\sec.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sec_add.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\sec_add.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\sec_add.obj"	"$(INTDIR)\sec_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\sec_add.obj"	"$(INTDIR)\sec_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\sec_add.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\sec_add.obj"	"$(INTDIR)\sec_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\sec_add.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\sec_add.obj"	"$(INTDIR)\sec_add.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\sec_add.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sec_allc.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\sec_allc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\sec_allc.obj"	"$(INTDIR)\sec_allc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\sec_allc.obj"	"$(INTDIR)\sec_allc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\sec_allc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\sec_allc.obj"	"$(INTDIR)\sec_allc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\sec_allc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\sec_allc.obj"	"$(INTDIR)\sec_allc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\sec_allc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sec_comp.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\sec_comp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\sec_comp.obj"	"$(INTDIR)\sec_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\sec_comp.obj"	"$(INTDIR)\sec_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\sec_comp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\sec_comp.obj"	"$(INTDIR)\sec_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\sec_comp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\sec_comp.obj"	"$(INTDIR)\sec_comp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\sec_comp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sec_make.c

!IF  "$(CFG)" == "Property - Win32 Release"


"$(INTDIR)\sec_make.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Debug"


"$(INTDIR)\sec_make.obj"	"$(INTDIR)\sec_make.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 DebugDll"


"$(INTDIR)\sec_make.obj"	"$(INTDIR)\sec_make.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 ReleaseDll"


"$(INTDIR)\sec_make.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode DebugDll"


"$(INTDIR)\sec_make.obj"	"$(INTDIR)\sec_make.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Release"


"$(INTDIR)\sec_make.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode Debug"


"$(INTDIR)\sec_make.obj"	"$(INTDIR)\sec_make.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Property - Win32 Unicode ReleaseDll"


"$(INTDIR)\sec_make.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


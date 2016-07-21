# Microsoft Developer Studio Generated NMAKE File, Based on rpc.dsp
!IF "$(CFG)" == ""
CFG=pxstub - Win32 Debug
!MESSAGE No configuration specified. Defaulting to pxstub - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pxstub - Win32 Release" && "$(CFG)" != "pxstub - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rpc.mak" CFG="pxstub - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pxstub - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pxstub - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pxstub - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/pxstub
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "./rpc/rpc_task_s.c" "./rpc/rpc_task_c.c" "./rpc/rpc_task.h" "./rpc/rpc_string_s.c" "./rpc/rpc_string_c.c" "./rpc/rpc_string.h" "./rpc/rpc_report_s.c" "./rpc/rpc_report_c.c" "./rpc/rpc_report.h" "./rpc/rpc_os_s.c" "./rpc/rpc_os_c.c" "./rpc/rpc_os.h" "./rpc/rpc_objptr_s.c" "./rpc/rpc_objptr_c.c" "./rpc/rpc_objptr.h" "./rpc/rpc_io_s.c" "./rpc/rpc_io_c.c" "./rpc/rpc_io.h" "./rpc/rpc_common_s.c" "./rpc/rpc_common_c.c" "./rpc/rpc_common.h" "./rpc/rpc_bl_s.c" "./rpc/rpc_bl_c.c" "./rpc/rpc_bl.h" "./rpc/rpc_avs_s.c" "./rpc/rpc_avs_c.c" "./rpc/rpc_avs.h" "$(OUTDIR)\pxstub.ppl"


CLEAN :
	-@erase "$(INTDIR)\avs_c.obj"
	-@erase "$(INTDIR)\avs_s.obj"
	-@erase "$(INTDIR)\bl_c.obj"
	-@erase "$(INTDIR)\bl_s.obj"
	-@erase "$(INTDIR)\common_c.obj"
	-@erase "$(INTDIR)\common_s.obj"
	-@erase "$(INTDIR)\io_c.obj"
	-@erase "$(INTDIR)\io_s.obj"
	-@erase "$(INTDIR)\objptr_c.obj"
	-@erase "$(INTDIR)\objptr_s.obj"
	-@erase "$(INTDIR)\os_c.obj"
	-@erase "$(INTDIR)\os_s.obj"
	-@erase "$(INTDIR)\plugin_rpc.obj"
	-@erase "$(INTDIR)\report_c.obj"
	-@erase "$(INTDIR)\report_s.obj"
	-@erase "$(INTDIR)\rpc.res"
	-@erase "$(INTDIR)\rpc_avs_c.obj"
	-@erase "$(INTDIR)\rpc_avs_s.obj"
	-@erase "$(INTDIR)\rpc_bl_c.obj"
	-@erase "$(INTDIR)\rpc_bl_s.obj"
	-@erase "$(INTDIR)\rpc_common_c.obj"
	-@erase "$(INTDIR)\rpc_common_s.obj"
	-@erase "$(INTDIR)\rpc_io_c.obj"
	-@erase "$(INTDIR)\rpc_io_s.obj"
	-@erase "$(INTDIR)\rpc_objptr_c.obj"
	-@erase "$(INTDIR)\rpc_objptr_s.obj"
	-@erase "$(INTDIR)\rpc_os_c.obj"
	-@erase "$(INTDIR)\rpc_os_s.obj"
	-@erase "$(INTDIR)\rpc_register.obj"
	-@erase "$(INTDIR)\rpc_report_c.obj"
	-@erase "$(INTDIR)\rpc_report_s.obj"
	-@erase "$(INTDIR)\rpc_string_c.obj"
	-@erase "$(INTDIR)\rpc_string_s.obj"
	-@erase "$(INTDIR)\rpc_task_c.obj"
	-@erase "$(INTDIR)\rpc_task_s.obj"
	-@erase "$(INTDIR)\string_c.obj"
	-@erase "$(INTDIR)\string_s.obj"
	-@erase "$(INTDIR)\task_c.obj"
	-@erase "$(INTDIR)\task_s.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pxstub.exp"
	-@erase "$(OUTDIR)\pxstub.pdb"
	-@erase "$(OUTDIR)\pxstub.ppl"
	-@erase "./rpc/rpc_avs.h"
	-@erase "./rpc/rpc_avs_c.c"
	-@erase "./rpc/rpc_avs_s.c"
	-@erase "./rpc/rpc_bl.h"
	-@erase "./rpc/rpc_bl_c.c"
	-@erase "./rpc/rpc_bl_s.c"
	-@erase "./rpc/rpc_common.h"
	-@erase "./rpc/rpc_common_c.c"
	-@erase "./rpc/rpc_common_s.c"
	-@erase "./rpc/rpc_io.h"
	-@erase "./rpc/rpc_io_c.c"
	-@erase "./rpc/rpc_io_s.c"
	-@erase "./rpc/rpc_objptr.h"
	-@erase "./rpc/rpc_objptr_c.c"
	-@erase "./rpc/rpc_objptr_s.c"
	-@erase "./rpc/rpc_os.h"
	-@erase "./rpc/rpc_os_c.c"
	-@erase "./rpc/rpc_os_s.c"
	-@erase "./rpc/rpc_report.h"
	-@erase "./rpc/rpc_report_c.c"
	-@erase "./rpc/rpc_report_s.c"
	-@erase "./rpc/rpc_string.h"
	-@erase "./rpc/rpc_string_c.c"
	-@erase "./rpc/rpc_string_s.c"
	-@erase "./rpc/rpc_task.h"
	-@erase "./rpc/rpc_task_c.c"
	-@erase "./rpc/rpc_task_s.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "./rpc" /I "../include" /I "../include/iface" /I "../../prague" /I "../../prague/include" /I "./" /I "../../prague/include/iface" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "rpc_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\rpc.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rpc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=stream.lib rpcrt4.lib /nologo /base:"0x66900000" /dll /incremental:no /pdb:"$(OUTDIR)\pxstub.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pxstub.ppl" /implib:"$(OUTDIR)\pxstub.lib" /pdbtype:sept /libpath:"../../CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\rpc_avs_c.obj" \
	"$(INTDIR)\rpc_avs_s.obj" \
	"$(INTDIR)\rpc_bl_c.obj" \
	"$(INTDIR)\rpc_bl_s.obj" \
	"$(INTDIR)\rpc_common_c.obj" \
	"$(INTDIR)\rpc_common_s.obj" \
	"$(INTDIR)\rpc_io_c.obj" \
	"$(INTDIR)\rpc_io_s.obj" \
	"$(INTDIR)\rpc_objptr_c.obj" \
	"$(INTDIR)\rpc_objptr_s.obj" \
	"$(INTDIR)\rpc_os_c.obj" \
	"$(INTDIR)\rpc_os_s.obj" \
	"$(INTDIR)\rpc_report_c.obj" \
	"$(INTDIR)\rpc_report_s.obj" \
	"$(INTDIR)\rpc_string_c.obj" \
	"$(INTDIR)\rpc_string_s.obj" \
	"$(INTDIR)\rpc_task_c.obj" \
	"$(INTDIR)\rpc_task_s.obj" \
	"$(INTDIR)\avs_c.obj" \
	"$(INTDIR)\avs_s.obj" \
	"$(INTDIR)\bl_c.obj" \
	"$(INTDIR)\bl_s.obj" \
	"$(INTDIR)\common_c.obj" \
	"$(INTDIR)\common_s.obj" \
	"$(INTDIR)\io_c.obj" \
	"$(INTDIR)\io_s.obj" \
	"$(INTDIR)\objptr_c.obj" \
	"$(INTDIR)\objptr_s.obj" \
	"$(INTDIR)\os_c.obj" \
	"$(INTDIR)\os_s.obj" \
	"$(INTDIR)\report_c.obj" \
	"$(INTDIR)\report_s.obj" \
	"$(INTDIR)\string_c.obj" \
	"$(INTDIR)\string_s.obj" \
	"$(INTDIR)\task_c.obj" \
	"$(INTDIR)\task_s.obj" \
	"$(INTDIR)\plugin_rpc.obj" \
	"$(INTDIR)\rpc_register.obj" \
	"$(INTDIR)\rpc.res"

"$(OUTDIR)\pxstub.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\pxstub.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "./rpc/rpc_task_s.c" "./rpc/rpc_task_c.c" "./rpc/rpc_task.h" "./rpc/rpc_string_s.c" "./rpc/rpc_string_c.c" "./rpc/rpc_string.h" "./rpc/rpc_report_s.c" "./rpc/rpc_report_c.c" "./rpc/rpc_report.h" "./rpc/rpc_os_s.c" "./rpc/rpc_os_c.c" "./rpc/rpc_os.h" "./rpc/rpc_objptr_s.c" "./rpc/rpc_objptr_c.c" "./rpc/rpc_objptr.h" "./rpc/rpc_io_s.c" "./rpc/rpc_io_c.c" "./rpc/rpc_io.h" "./rpc/rpc_common_s.c" "./rpc/rpc_common_c.c" "./rpc/rpc_common.h" "./rpc/rpc_bl_s.c" "./rpc/rpc_bl_c.c" "./rpc/rpc_bl.h" "./rpc/rpc_avs_s.c" "./rpc/rpc_avs_c.c" "./rpc/rpc_avs.h" "$(OUTDIR)\pxstub.ppl"
   tsigner "\out\Release\pxstub.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/pxstub
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "./rpc/rpc_task_s.c" "./rpc/rpc_task_c.c" "./rpc/rpc_task.h" "./rpc/rpc_string_s.c" "./rpc/rpc_string_c.c" "./rpc/rpc_string.h" "./rpc/rpc_report_s.c" "./rpc/rpc_report_c.c" "./rpc/rpc_report.h" "./rpc/rpc_os_s.c" "./rpc/rpc_os_c.c" "./rpc/rpc_os.h" "./rpc/rpc_objptr_s.c" "./rpc/rpc_objptr_c.c" "./rpc/rpc_objptr.h" "./rpc/rpc_io_s.c" "./rpc/rpc_io_c.c" "./rpc/rpc_io.h" "./rpc/rpc_common_s.c" "./rpc/rpc_common_c.c" "./rpc/rpc_common.h" "./rpc/rpc_bl_s.c" "./rpc/rpc_bl_c.c" "./rpc/rpc_bl.h" "./rpc/rpc_avs_s.c" "./rpc/rpc_avs_c.c" "./rpc/rpc_avs.h" "$(OUTDIR)\pxstub.ppl" "$(OUTDIR)\rpc.bsc"


CLEAN :
	-@erase "$(INTDIR)\avs_c.obj"
	-@erase "$(INTDIR)\avs_c.sbr"
	-@erase "$(INTDIR)\avs_s.obj"
	-@erase "$(INTDIR)\avs_s.sbr"
	-@erase "$(INTDIR)\bl_c.obj"
	-@erase "$(INTDIR)\bl_c.sbr"
	-@erase "$(INTDIR)\bl_s.obj"
	-@erase "$(INTDIR)\bl_s.sbr"
	-@erase "$(INTDIR)\common_c.obj"
	-@erase "$(INTDIR)\common_c.sbr"
	-@erase "$(INTDIR)\common_s.obj"
	-@erase "$(INTDIR)\common_s.sbr"
	-@erase "$(INTDIR)\io_c.obj"
	-@erase "$(INTDIR)\io_c.sbr"
	-@erase "$(INTDIR)\io_s.obj"
	-@erase "$(INTDIR)\io_s.sbr"
	-@erase "$(INTDIR)\objptr_c.obj"
	-@erase "$(INTDIR)\objptr_c.sbr"
	-@erase "$(INTDIR)\objptr_s.obj"
	-@erase "$(INTDIR)\objptr_s.sbr"
	-@erase "$(INTDIR)\os_c.obj"
	-@erase "$(INTDIR)\os_c.sbr"
	-@erase "$(INTDIR)\os_s.obj"
	-@erase "$(INTDIR)\os_s.sbr"
	-@erase "$(INTDIR)\plugin_rpc.obj"
	-@erase "$(INTDIR)\plugin_rpc.sbr"
	-@erase "$(INTDIR)\report_c.obj"
	-@erase "$(INTDIR)\report_c.sbr"
	-@erase "$(INTDIR)\report_s.obj"
	-@erase "$(INTDIR)\report_s.sbr"
	-@erase "$(INTDIR)\rpc.res"
	-@erase "$(INTDIR)\rpc_avs_c.obj"
	-@erase "$(INTDIR)\rpc_avs_c.sbr"
	-@erase "$(INTDIR)\rpc_avs_s.obj"
	-@erase "$(INTDIR)\rpc_avs_s.sbr"
	-@erase "$(INTDIR)\rpc_bl_c.obj"
	-@erase "$(INTDIR)\rpc_bl_c.sbr"
	-@erase "$(INTDIR)\rpc_bl_s.obj"
	-@erase "$(INTDIR)\rpc_bl_s.sbr"
	-@erase "$(INTDIR)\rpc_common_c.obj"
	-@erase "$(INTDIR)\rpc_common_c.sbr"
	-@erase "$(INTDIR)\rpc_common_s.obj"
	-@erase "$(INTDIR)\rpc_common_s.sbr"
	-@erase "$(INTDIR)\rpc_io_c.obj"
	-@erase "$(INTDIR)\rpc_io_c.sbr"
	-@erase "$(INTDIR)\rpc_io_s.obj"
	-@erase "$(INTDIR)\rpc_io_s.sbr"
	-@erase "$(INTDIR)\rpc_objptr_c.obj"
	-@erase "$(INTDIR)\rpc_objptr_c.sbr"
	-@erase "$(INTDIR)\rpc_objptr_s.obj"
	-@erase "$(INTDIR)\rpc_objptr_s.sbr"
	-@erase "$(INTDIR)\rpc_os_c.obj"
	-@erase "$(INTDIR)\rpc_os_c.sbr"
	-@erase "$(INTDIR)\rpc_os_s.obj"
	-@erase "$(INTDIR)\rpc_os_s.sbr"
	-@erase "$(INTDIR)\rpc_register.obj"
	-@erase "$(INTDIR)\rpc_register.sbr"
	-@erase "$(INTDIR)\rpc_report_c.obj"
	-@erase "$(INTDIR)\rpc_report_c.sbr"
	-@erase "$(INTDIR)\rpc_report_s.obj"
	-@erase "$(INTDIR)\rpc_report_s.sbr"
	-@erase "$(INTDIR)\rpc_string_c.obj"
	-@erase "$(INTDIR)\rpc_string_c.sbr"
	-@erase "$(INTDIR)\rpc_string_s.obj"
	-@erase "$(INTDIR)\rpc_string_s.sbr"
	-@erase "$(INTDIR)\rpc_task_c.obj"
	-@erase "$(INTDIR)\rpc_task_c.sbr"
	-@erase "$(INTDIR)\rpc_task_s.obj"
	-@erase "$(INTDIR)\rpc_task_s.sbr"
	-@erase "$(INTDIR)\string_c.obj"
	-@erase "$(INTDIR)\string_c.sbr"
	-@erase "$(INTDIR)\string_s.obj"
	-@erase "$(INTDIR)\string_s.sbr"
	-@erase "$(INTDIR)\task_c.obj"
	-@erase "$(INTDIR)\task_c.sbr"
	-@erase "$(INTDIR)\task_s.obj"
	-@erase "$(INTDIR)\task_s.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pxstub.exp"
	-@erase "$(OUTDIR)\pxstub.ilk"
	-@erase "$(OUTDIR)\pxstub.pdb"
	-@erase "$(OUTDIR)\pxstub.ppl"
	-@erase "$(OUTDIR)\rpc.bsc"
	-@erase "./rpc/rpc_avs.h"
	-@erase "./rpc/rpc_avs_c.c"
	-@erase "./rpc/rpc_avs_s.c"
	-@erase "./rpc/rpc_bl.h"
	-@erase "./rpc/rpc_bl_c.c"
	-@erase "./rpc/rpc_bl_s.c"
	-@erase "./rpc/rpc_common.h"
	-@erase "./rpc/rpc_common_c.c"
	-@erase "./rpc/rpc_common_s.c"
	-@erase "./rpc/rpc_io.h"
	-@erase "./rpc/rpc_io_c.c"
	-@erase "./rpc/rpc_io_s.c"
	-@erase "./rpc/rpc_objptr.h"
	-@erase "./rpc/rpc_objptr_c.c"
	-@erase "./rpc/rpc_objptr_s.c"
	-@erase "./rpc/rpc_os.h"
	-@erase "./rpc/rpc_os_c.c"
	-@erase "./rpc/rpc_os_s.c"
	-@erase "./rpc/rpc_report.h"
	-@erase "./rpc/rpc_report_c.c"
	-@erase "./rpc/rpc_report_s.c"
	-@erase "./rpc/rpc_string.h"
	-@erase "./rpc/rpc_string_c.c"
	-@erase "./rpc/rpc_string_s.c"
	-@erase "./rpc/rpc_task.h"
	-@erase "./rpc/rpc_task_c.c"
	-@erase "./rpc/rpc_task_s.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./rpc" /I "../include" /I "../include/iface" /I "../../prague" /I "../../prague/include" /I "./" /I "../../prague/include/iface" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "rpc_EXPORTS" /D "C_STYLE_PROP" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\rpc.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rpc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\rpc_avs_c.sbr" \
	"$(INTDIR)\rpc_avs_s.sbr" \
	"$(INTDIR)\rpc_bl_c.sbr" \
	"$(INTDIR)\rpc_bl_s.sbr" \
	"$(INTDIR)\rpc_common_c.sbr" \
	"$(INTDIR)\rpc_common_s.sbr" \
	"$(INTDIR)\rpc_io_c.sbr" \
	"$(INTDIR)\rpc_io_s.sbr" \
	"$(INTDIR)\rpc_objptr_c.sbr" \
	"$(INTDIR)\rpc_objptr_s.sbr" \
	"$(INTDIR)\rpc_os_c.sbr" \
	"$(INTDIR)\rpc_os_s.sbr" \
	"$(INTDIR)\rpc_report_c.sbr" \
	"$(INTDIR)\rpc_report_s.sbr" \
	"$(INTDIR)\rpc_string_c.sbr" \
	"$(INTDIR)\rpc_string_s.sbr" \
	"$(INTDIR)\rpc_task_c.sbr" \
	"$(INTDIR)\rpc_task_s.sbr" \
	"$(INTDIR)\avs_c.sbr" \
	"$(INTDIR)\avs_s.sbr" \
	"$(INTDIR)\bl_c.sbr" \
	"$(INTDIR)\bl_s.sbr" \
	"$(INTDIR)\common_c.sbr" \
	"$(INTDIR)\common_s.sbr" \
	"$(INTDIR)\io_c.sbr" \
	"$(INTDIR)\io_s.sbr" \
	"$(INTDIR)\objptr_c.sbr" \
	"$(INTDIR)\objptr_s.sbr" \
	"$(INTDIR)\os_c.sbr" \
	"$(INTDIR)\os_s.sbr" \
	"$(INTDIR)\report_c.sbr" \
	"$(INTDIR)\report_s.sbr" \
	"$(INTDIR)\string_c.sbr" \
	"$(INTDIR)\string_s.sbr" \
	"$(INTDIR)\task_c.sbr" \
	"$(INTDIR)\task_s.sbr" \
	"$(INTDIR)\plugin_rpc.sbr" \
	"$(INTDIR)\rpc_register.sbr"

"$(OUTDIR)\rpc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=stream.lib rpcrt4.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\pxstub.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pxstub.ppl" /implib:"$(OUTDIR)\pxstub.lib" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\rpc_avs_c.obj" \
	"$(INTDIR)\rpc_avs_s.obj" \
	"$(INTDIR)\rpc_bl_c.obj" \
	"$(INTDIR)\rpc_bl_s.obj" \
	"$(INTDIR)\rpc_common_c.obj" \
	"$(INTDIR)\rpc_common_s.obj" \
	"$(INTDIR)\rpc_io_c.obj" \
	"$(INTDIR)\rpc_io_s.obj" \
	"$(INTDIR)\rpc_objptr_c.obj" \
	"$(INTDIR)\rpc_objptr_s.obj" \
	"$(INTDIR)\rpc_os_c.obj" \
	"$(INTDIR)\rpc_os_s.obj" \
	"$(INTDIR)\rpc_report_c.obj" \
	"$(INTDIR)\rpc_report_s.obj" \
	"$(INTDIR)\rpc_string_c.obj" \
	"$(INTDIR)\rpc_string_s.obj" \
	"$(INTDIR)\rpc_task_c.obj" \
	"$(INTDIR)\rpc_task_s.obj" \
	"$(INTDIR)\avs_c.obj" \
	"$(INTDIR)\avs_s.obj" \
	"$(INTDIR)\bl_c.obj" \
	"$(INTDIR)\bl_s.obj" \
	"$(INTDIR)\common_c.obj" \
	"$(INTDIR)\common_s.obj" \
	"$(INTDIR)\io_c.obj" \
	"$(INTDIR)\io_s.obj" \
	"$(INTDIR)\objptr_c.obj" \
	"$(INTDIR)\objptr_s.obj" \
	"$(INTDIR)\os_c.obj" \
	"$(INTDIR)\os_s.obj" \
	"$(INTDIR)\report_c.obj" \
	"$(INTDIR)\report_s.obj" \
	"$(INTDIR)\string_c.obj" \
	"$(INTDIR)\string_s.obj" \
	"$(INTDIR)\task_c.obj" \
	"$(INTDIR)\task_s.obj" \
	"$(INTDIR)\plugin_rpc.obj" \
	"$(INTDIR)\rpc_register.obj" \
	"$(INTDIR)\rpc.res"

"$(OUTDIR)\pxstub.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("rpc.dep")
!INCLUDE "rpc.dep"
!ELSE 
!MESSAGE Warning: cannot find "rpc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pxstub - Win32 Release" || "$(CFG)" == "pxstub - Win32 Debug"
SOURCE=.\idl\rpc_avs.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=.\idl\rpc_avs.idl
InputName=rpc_avs

".\rpc\rpc_avs_c.c"	".\rpc\rpc_avs_s.c"	".\rpc\rpc_avs.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=.\idl\rpc_avs.idl
InputName=rpc_avs

".\rpc\rpc_avs_c.c"	".\rpc\rpc_avs_s.c"	".\rpc\rpc_avs.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\idl\rpc_bl.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=.\idl\rpc_bl.idl
InputName=rpc_bl

".\rpc\rpc_bl_c.c"	".\rpc\rpc_bl_s.c"	".\rpc\rpc_bl.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=.\idl\rpc_bl.idl
InputName=rpc_bl

".\rpc\rpc_bl_c.c"	".\rpc\rpc_bl_s.c"	".\rpc\rpc_bl.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\IDL\rpc_common.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=.\IDL\rpc_common.idl
InputName=rpc_common

".\RPC\rpc_common_c.c"	".\RPC\rpc_common_s.c"	".\RPC\rpc_common.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=.\IDL\rpc_common.idl
InputName=rpc_common

".\RPC\rpc_common_c.c"	".\RPC\rpc_common_s.c"	".\RPC\rpc_common.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=..\..\Prague\remote\idl\rpc_io.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=..\..\Prague\remote\idl\rpc_io.idl
InputName=rpc_io

".\rpc\rpc_io_c.c"	".\rpc\rpc_io_s.c"	".\rpc\rpc_io.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=..\..\Prague\remote\idl\rpc_io.idl
InputName=rpc_io

".\rpc\rpc_io_c.c"	".\rpc\rpc_io_s.c"	".\rpc\rpc_io.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=..\..\Prague\remote\idl\rpc_objptr.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=..\..\Prague\remote\idl\rpc_objptr.idl
InputName=rpc_objptr

".\rpc\rpc_objptr_c.c"	".\rpc\rpc_objptr_s.c"	".\rpc\rpc_objptr.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=..\..\Prague\remote\idl\rpc_objptr.idl
InputName=rpc_objptr

".\rpc\rpc_objptr_c.c"	".\rpc\rpc_objptr_s.c"	".\rpc\rpc_objptr.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=..\..\Prague\remote\idl\rpc_os.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=..\..\Prague\remote\idl\rpc_os.idl
InputName=rpc_os

".\rpc\rpc_os_c.c"	".\rpc\rpc_os_s.c"	".\rpc\rpc_os.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=..\..\Prague\remote\idl\rpc_os.idl
InputName=rpc_os

".\rpc\rpc_os_c.c"	".\rpc\rpc_os_s.c"	".\rpc\rpc_os.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\idl\rpc_report.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=.\idl\rpc_report.idl
InputName=rpc_report

".\rpc\rpc_report_c.c"	".\rpc\rpc_report_s.c"	".\rpc\rpc_report.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=.\idl\rpc_report.idl
InputName=rpc_report

".\rpc\rpc_report_c.c"	".\rpc\rpc_report_s.c"	".\rpc\rpc_report.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=..\..\prague\remote\idl\rpc_string.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=..\..\prague\remote\idl\rpc_string.idl
InputName=rpc_string

".\RPC\rpc_string_c.c"	".\RPC\rpc_string_s.c"	".\rpc\rpc_string.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=..\..\prague\remote\idl\rpc_string.idl
InputName=rpc_string

".\RPC\rpc_string_c.c"	".\RPC\rpc_string_s.c"	".\rpc\rpc_string.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ../../prague/remote/idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\IDL\rpc_task.idl

!IF  "$(CFG)" == "pxstub - Win32 Release"

InputPath=.\IDL\rpc_task.idl
InputName=rpc_task

".\RPC\rpc_task_c.c"	".\RPC\rpc_task_s.c"	".\RPC\rpc_task.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"

InputPath=.\IDL\rpc_task.idl
InputName=rpc_task

".\RPC\rpc_task_c.c"	".\RPC\rpc_task_s.c"	".\RPC\rpc_task.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -out ./rpc -acf ./idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc\rpc_avs_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_avs_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_avs_c.obj"	"$(INTDIR)\rpc_avs_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_avs_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_avs_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_avs_s.obj"	"$(INTDIR)\rpc_avs_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_bl_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_bl_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_bl_c.obj"	"$(INTDIR)\rpc_bl_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_bl_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_bl_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_bl_s.obj"	"$(INTDIR)\rpc_bl_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\rpc_common_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_common_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_common_c.obj"	"$(INTDIR)\rpc_common_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\rpc_common_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_common_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_common_s.obj"	"$(INTDIR)\rpc_common_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_io_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_io_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_io_c.obj"	"$(INTDIR)\rpc_io_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_io_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_io_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_io_s.obj"	"$(INTDIR)\rpc_io_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_objptr_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_objptr_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_objptr_c.obj"	"$(INTDIR)\rpc_objptr_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_objptr_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_objptr_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_objptr_s.obj"	"$(INTDIR)\rpc_objptr_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_os_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_os_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_os_c.obj"	"$(INTDIR)\rpc_os_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_os_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_os_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_os_s.obj"	"$(INTDIR)\rpc_os_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_report_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_report_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_report_c.obj"	"$(INTDIR)\rpc_report_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\rpc_report_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_report_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_report_s.obj"	"$(INTDIR)\rpc_report_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\rpc_string_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_string_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_string_c.obj"	"$(INTDIR)\rpc_string_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\rpc_string_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_string_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_string_s.obj"	"$(INTDIR)\rpc_string_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\rpc_task_c.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_task_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_task_c.obj"	"$(INTDIR)\rpc_task_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\rpc_task_s.c

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_task_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_task_s.obj"	"$(INTDIR)\rpc_task_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\avs_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\avs_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\avs_c.obj"	"$(INTDIR)\avs_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\avs_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\avs_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\avs_s.obj"	"$(INTDIR)\avs_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\bl_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\bl_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\bl_c.obj"	"$(INTDIR)\bl_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\bl_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\bl_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\bl_s.obj"	"$(INTDIR)\bl_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\common_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\common_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\common_c.obj"	"$(INTDIR)\common_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\common_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\common_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\common_s.obj"	"$(INTDIR)\common_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Prague\remote\pr_client\io_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\io_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\io_c.obj"	"$(INTDIR)\io_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Prague\remote\pr_server\io_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\io_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\io_s.obj"	"$(INTDIR)\io_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Prague\remote\pr_client\objptr_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\objptr_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\objptr_c.obj"	"$(INTDIR)\objptr_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Prague\remote\pr_server\objptr_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\objptr_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\objptr_s.obj"	"$(INTDIR)\objptr_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Prague\remote\pr_client\os_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\os_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\os_c.obj"	"$(INTDIR)\os_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Prague\remote\pr_server\os_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\os_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\os_s.obj"	"$(INTDIR)\os_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\report_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\report_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\report_c.obj"	"$(INTDIR)\report_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\rpc\report_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\report_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\report_s.obj"	"$(INTDIR)\report_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\prague\remote\pr_client\string_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\string_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\string_c.obj"	"$(INTDIR)\string_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\prague\remote\pr_server\string_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\string_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\string_s.obj"	"$(INTDIR)\string_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\task_c.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\task_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\task_c.obj"	"$(INTDIR)\task_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\RPC\task_s.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\task_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\task_s.obj"	"$(INTDIR)\task_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\plugin_rpc.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\plugin_rpc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\plugin_rpc.obj"	"$(INTDIR)\plugin_rpc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rpc_register.cpp

!IF  "$(CFG)" == "pxstub - Win32 Release"


"$(INTDIR)\rpc_register.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pxstub - Win32 Debug"


"$(INTDIR)\rpc_register.obj"	"$(INTDIR)\rpc_register.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rpc.rc

"$(INTDIR)\rpc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


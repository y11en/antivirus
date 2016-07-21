# Microsoft Developer Studio Generated NMAKE File, Based on pr_remote.dsp
!IF "$(CFG)" == ""
CFG=pr_remote - Win32 Debug MBCS
!MESSAGE No configuration specified. Defaulting to pr_remote - Win32 Debug MBCS.
!ENDIF 

!IF "$(CFG)" != "pr_remote - Win32 Release" && "$(CFG)" != "pr_remote - Win32 Debug" && "$(CFG)" != "pr_remote - Win32 Debug MBCS" && "$(CFG)" != "pr_remote - Win32 Release MBCS"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pr_remote.mak" CFG="pr_remote - Win32 Debug MBCS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pr_remote - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Debug MBCS" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_remote - Win32 Release MBCS" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pr_remote - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/remote
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : ".\rpc_root_s.c" ".\rpc_root_c.c" ".\rpc_root.h" ".\rpc_remote_s.c" ".\rpc_remote_c.c" ".\rpc_remote.h" ".\pr_system/rpc_system_s.c" ".\pr_system/rpc_system_c.c" ".\pr_system/rpc_system.h" "$(OUTDIR)\pr_remote.dll"


CLEAN :
	-@erase "$(INTDIR)\bl_component.obj"
	-@erase "$(INTDIR)\pr_internal.obj"
	-@erase "$(INTDIR)\pr_library.obj"
	-@erase "$(INTDIR)\pr_remote.obj"
	-@erase "$(INTDIR)\pr_remote.res"
	-@erase "$(INTDIR)\pr_root_c.obj"
	-@erase "$(INTDIR)\pr_root_s.obj"
	-@erase "$(INTDIR)\pr_system_c.obj"
	-@erase "$(INTDIR)\pr_system_s.obj"
	-@erase "$(INTDIR)\rpc_connect.obj"
	-@erase "$(INTDIR)\rpc_remote_c.obj"
	-@erase "$(INTDIR)\rpc_remote_s.obj"
	-@erase "$(INTDIR)\rpc_root_c.obj"
	-@erase "$(INTDIR)\rpc_root_s.obj"
	-@erase "$(INTDIR)\rpc_system_c.obj"
	-@erase "$(INTDIR)\rpc_system_s.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\SharedTable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_remote.dll"
	-@erase "$(OUTDIR)\pr_remote.exp"
	-@erase "$(OUTDIR)\pr_remote.pdb"
	-@erase "pr_system/rpc_system.h"
	-@erase "pr_system/rpc_system_c.c"
	-@erase "pr_system/rpc_system_s.c"
	-@erase "rpc_remote.h"
	-@erase "rpc_remote_c.c"
	-@erase "rpc_remote_s.c"
	-@erase "rpc_root.h"
	-@erase "rpc_root_c.c"
	-@erase "rpc_root_s.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_remote.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_remote.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\pr_remote.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_remote.dll" /implib:"$(OUTDIR)\pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\LIB" /libpath:"..\release" /libpath:"..\..\CommonFiles\ReleaseDll" /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\pr_system_c.obj" \
	"$(INTDIR)\pr_system_s.obj" \
	"$(INTDIR)\rpc_system_c.obj" \
	"$(INTDIR)\rpc_system_s.obj" \
	"$(INTDIR)\bl_component.obj" \
	"$(INTDIR)\pr_internal.obj" \
	"$(INTDIR)\pr_library.obj" \
	"$(INTDIR)\pr_remote.obj" \
	"$(INTDIR)\pr_root_c.obj" \
	"$(INTDIR)\pr_root_s.obj" \
	"$(INTDIR)\rpc_connect.obj" \
	"$(INTDIR)\rpc_remote_c.obj" \
	"$(INTDIR)\rpc_remote_s.obj" \
	"$(INTDIR)\rpc_root_c.obj" \
	"$(INTDIR)\rpc_root_s.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\SharedTable.obj" \
	"$(INTDIR)\pr_remote.res"

"$(OUTDIR)\pr_remote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\pr_remote.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : ".\rpc_root_s.c" ".\rpc_root_c.c" ".\rpc_root.h" ".\rpc_remote_s.c" ".\rpc_remote_c.c" ".\rpc_remote.h" ".\pr_system/rpc_system_s.c" ".\pr_system/rpc_system_c.c" ".\pr_system/rpc_system.h" "$(OUTDIR)\pr_remote.dll"
   tsigner "\out\Release\pr_remote.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/remote
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : ".\rpc_root_s.c" ".\rpc_root_c.c" ".\rpc_root.h" ".\rpc_remote_s.c" ".\rpc_remote_c.c" ".\rpc_remote.h" ".\pr_system/rpc_system_s.c" ".\pr_system/rpc_system_c.c" ".\pr_system/rpc_system.h" "$(OUTDIR)\pr_remote.dll" "$(OUTDIR)\pr_remote.bsc"


CLEAN :
	-@erase "$(INTDIR)\bl_component.obj"
	-@erase "$(INTDIR)\bl_component.sbr"
	-@erase "$(INTDIR)\pr_internal.obj"
	-@erase "$(INTDIR)\pr_internal.sbr"
	-@erase "$(INTDIR)\pr_library.obj"
	-@erase "$(INTDIR)\pr_library.sbr"
	-@erase "$(INTDIR)\pr_remote.obj"
	-@erase "$(INTDIR)\pr_remote.res"
	-@erase "$(INTDIR)\pr_remote.sbr"
	-@erase "$(INTDIR)\pr_root_c.obj"
	-@erase "$(INTDIR)\pr_root_c.sbr"
	-@erase "$(INTDIR)\pr_root_s.obj"
	-@erase "$(INTDIR)\pr_root_s.sbr"
	-@erase "$(INTDIR)\pr_system_c.obj"
	-@erase "$(INTDIR)\pr_system_c.sbr"
	-@erase "$(INTDIR)\pr_system_s.obj"
	-@erase "$(INTDIR)\pr_system_s.sbr"
	-@erase "$(INTDIR)\rpc_connect.obj"
	-@erase "$(INTDIR)\rpc_connect.sbr"
	-@erase "$(INTDIR)\rpc_remote_c.obj"
	-@erase "$(INTDIR)\rpc_remote_c.sbr"
	-@erase "$(INTDIR)\rpc_remote_s.obj"
	-@erase "$(INTDIR)\rpc_remote_s.sbr"
	-@erase "$(INTDIR)\rpc_root_c.obj"
	-@erase "$(INTDIR)\rpc_root_c.sbr"
	-@erase "$(INTDIR)\rpc_root_s.obj"
	-@erase "$(INTDIR)\rpc_root_s.sbr"
	-@erase "$(INTDIR)\rpc_system_c.obj"
	-@erase "$(INTDIR)\rpc_system_c.sbr"
	-@erase "$(INTDIR)\rpc_system_s.obj"
	-@erase "$(INTDIR)\rpc_system_s.sbr"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sa.sbr"
	-@erase "$(INTDIR)\SharedTable.obj"
	-@erase "$(INTDIR)\SharedTable.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_remote.bsc"
	-@erase "$(OUTDIR)\pr_remote.dll"
	-@erase "$(OUTDIR)\pr_remote.exp"
	-@erase "$(OUTDIR)\pr_remote.ilk"
	-@erase "$(OUTDIR)\pr_remote.lib"
	-@erase "$(OUTDIR)\pr_remote.pdb"
	-@erase "pr_system/rpc_system.h"
	-@erase "pr_system/rpc_system_c.c"
	-@erase "pr_system/rpc_system_s.c"
	-@erase "rpc_remote.h"
	-@erase "rpc_remote_c.c"
	-@erase "rpc_remote_s.c"
	-@erase "rpc_root.h"
	-@erase "rpc_root_c.c"
	-@erase "rpc_root_s.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_remote.res" /i "../../commonfiles" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_remote.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\pr_system_c.sbr" \
	"$(INTDIR)\pr_system_s.sbr" \
	"$(INTDIR)\rpc_system_c.sbr" \
	"$(INTDIR)\rpc_system_s.sbr" \
	"$(INTDIR)\bl_component.sbr" \
	"$(INTDIR)\pr_internal.sbr" \
	"$(INTDIR)\pr_library.sbr" \
	"$(INTDIR)\pr_remote.sbr" \
	"$(INTDIR)\pr_root_c.sbr" \
	"$(INTDIR)\pr_root_s.sbr" \
	"$(INTDIR)\rpc_connect.sbr" \
	"$(INTDIR)\rpc_remote_c.sbr" \
	"$(INTDIR)\rpc_remote_s.sbr" \
	"$(INTDIR)\rpc_root_c.sbr" \
	"$(INTDIR)\rpc_root_s.sbr" \
	"$(INTDIR)\sa.sbr" \
	"$(INTDIR)\SharedTable.sbr"

"$(OUTDIR)\pr_remote.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\pr_remote.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_remote.dll" /implib:"$(OUTDIR)\pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\LIBD" /libpath:"..\debug" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\pr_system_c.obj" \
	"$(INTDIR)\pr_system_s.obj" \
	"$(INTDIR)\rpc_system_c.obj" \
	"$(INTDIR)\rpc_system_s.obj" \
	"$(INTDIR)\bl_component.obj" \
	"$(INTDIR)\pr_internal.obj" \
	"$(INTDIR)\pr_library.obj" \
	"$(INTDIR)\pr_remote.obj" \
	"$(INTDIR)\pr_root_c.obj" \
	"$(INTDIR)\pr_root_s.obj" \
	"$(INTDIR)\rpc_connect.obj" \
	"$(INTDIR)\rpc_remote_c.obj" \
	"$(INTDIR)\rpc_remote_s.obj" \
	"$(INTDIR)\rpc_root_c.obj" \
	"$(INTDIR)\rpc_root_s.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\SharedTable.obj" \
	"$(INTDIR)\pr_remote.res"

"$(OUTDIR)\pr_remote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

OUTDIR=.\../../out/Debug MBCS
INTDIR=.\../../temp/Debug MBCS/prague/remote
# Begin Custom Macros
OutDir=.\../../out/Debug MBCS
# End Custom Macros

ALL : "$(OUTDIR)\pr_remote.dll" "$(OUTDIR)\pr_remote.bsc"


CLEAN :
	-@erase "$(INTDIR)\bl_component.obj"
	-@erase "$(INTDIR)\bl_component.sbr"
	-@erase "$(INTDIR)\pr_internal.obj"
	-@erase "$(INTDIR)\pr_internal.sbr"
	-@erase "$(INTDIR)\pr_library.obj"
	-@erase "$(INTDIR)\pr_library.sbr"
	-@erase "$(INTDIR)\pr_remote.obj"
	-@erase "$(INTDIR)\pr_remote.res"
	-@erase "$(INTDIR)\pr_remote.sbr"
	-@erase "$(INTDIR)\pr_root_c.obj"
	-@erase "$(INTDIR)\pr_root_c.sbr"
	-@erase "$(INTDIR)\pr_root_s.obj"
	-@erase "$(INTDIR)\pr_root_s.sbr"
	-@erase "$(INTDIR)\pr_system_c.obj"
	-@erase "$(INTDIR)\pr_system_c.sbr"
	-@erase "$(INTDIR)\pr_system_s.obj"
	-@erase "$(INTDIR)\pr_system_s.sbr"
	-@erase "$(INTDIR)\rpc_connect.obj"
	-@erase "$(INTDIR)\rpc_connect.sbr"
	-@erase "$(INTDIR)\rpc_remote_c.obj"
	-@erase "$(INTDIR)\rpc_remote_c.sbr"
	-@erase "$(INTDIR)\rpc_remote_s.obj"
	-@erase "$(INTDIR)\rpc_remote_s.sbr"
	-@erase "$(INTDIR)\rpc_root_c.obj"
	-@erase "$(INTDIR)\rpc_root_c.sbr"
	-@erase "$(INTDIR)\rpc_root_s.obj"
	-@erase "$(INTDIR)\rpc_root_s.sbr"
	-@erase "$(INTDIR)\rpc_system_c.obj"
	-@erase "$(INTDIR)\rpc_system_c.sbr"
	-@erase "$(INTDIR)\rpc_system_s.obj"
	-@erase "$(INTDIR)\rpc_system_s.sbr"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sa.sbr"
	-@erase "$(INTDIR)\SharedTable.obj"
	-@erase "$(INTDIR)\SharedTable.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_remote.bsc"
	-@erase "$(OUTDIR)\pr_remote.dll"
	-@erase "$(OUTDIR)\pr_remote.ilk"
	-@erase "$(OUTDIR)\pr_remote.pdb"
	-@erase "..\Debug\pr_remote.exp"
	-@erase "..\Debug\pr_remote.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_remote.res" /i "../../commonfiles" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_remote.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\pr_system_c.sbr" \
	"$(INTDIR)\pr_system_s.sbr" \
	"$(INTDIR)\rpc_system_c.sbr" \
	"$(INTDIR)\rpc_system_s.sbr" \
	"$(INTDIR)\bl_component.sbr" \
	"$(INTDIR)\pr_internal.sbr" \
	"$(INTDIR)\pr_library.sbr" \
	"$(INTDIR)\pr_remote.sbr" \
	"$(INTDIR)\pr_root_c.sbr" \
	"$(INTDIR)\pr_root_s.sbr" \
	"$(INTDIR)\rpc_connect.sbr" \
	"$(INTDIR)\rpc_remote_c.sbr" \
	"$(INTDIR)\rpc_remote_s.sbr" \
	"$(INTDIR)\rpc_root_c.sbr" \
	"$(INTDIR)\rpc_root_s.sbr" \
	"$(INTDIR)\sa.sbr" \
	"$(INTDIR)\SharedTable.sbr"

"$(OUTDIR)\pr_remote.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\pr_remote.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_remote.dll" /implib:"../Debug/pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\NULIBD" /libpath:"..\debug" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\pr_system_c.obj" \
	"$(INTDIR)\pr_system_s.obj" \
	"$(INTDIR)\rpc_system_c.obj" \
	"$(INTDIR)\rpc_system_s.obj" \
	"$(INTDIR)\bl_component.obj" \
	"$(INTDIR)\pr_internal.obj" \
	"$(INTDIR)\pr_library.obj" \
	"$(INTDIR)\pr_remote.obj" \
	"$(INTDIR)\pr_root_c.obj" \
	"$(INTDIR)\pr_root_s.obj" \
	"$(INTDIR)\rpc_connect.obj" \
	"$(INTDIR)\rpc_remote_c.obj" \
	"$(INTDIR)\rpc_remote_s.obj" \
	"$(INTDIR)\rpc_root_c.obj" \
	"$(INTDIR)\rpc_root_s.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\SharedTable.obj" \
	"$(INTDIR)\pr_remote.res"

"$(OUTDIR)\pr_remote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Debug MBCS\pr_remote.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Debug MBCS
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\pr_remote.dll" "$(OUTDIR)\pr_remote.bsc"
   tsigner "\out\Debug MBCS\pr_remote.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

OUTDIR=.\../../out/Release MBCS
INTDIR=.\../../temp/Release MBCS/prague/remote
# Begin Custom Macros
OutDir=.\../../out/Release MBCS
# End Custom Macros

ALL : "$(OUTDIR)\pr_remote.dll"


CLEAN :
	-@erase "$(INTDIR)\bl_component.obj"
	-@erase "$(INTDIR)\pr_internal.obj"
	-@erase "$(INTDIR)\pr_library.obj"
	-@erase "$(INTDIR)\pr_remote.obj"
	-@erase "$(INTDIR)\pr_remote.res"
	-@erase "$(INTDIR)\pr_root_c.obj"
	-@erase "$(INTDIR)\pr_root_s.obj"
	-@erase "$(INTDIR)\pr_system_c.obj"
	-@erase "$(INTDIR)\pr_system_s.obj"
	-@erase "$(INTDIR)\rpc_connect.obj"
	-@erase "$(INTDIR)\rpc_remote_c.obj"
	-@erase "$(INTDIR)\rpc_remote_s.obj"
	-@erase "$(INTDIR)\rpc_root_c.obj"
	-@erase "$(INTDIR)\rpc_root_s.obj"
	-@erase "$(INTDIR)\rpc_system_c.obj"
	-@erase "$(INTDIR)\rpc_system_s.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\SharedTable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_remote.dll"
	-@erase "$(OUTDIR)\pr_remote.pdb"
	-@erase "..\Release\pr_remote.exp"
	-@erase "..\Release\pr_remote.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "./idl" /I "..\include" /I "..\include\iface" /I "../../commonfiles" /I "../../cs adminkit/development" /I "../../cs adminkit/development/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DISTRIBUTED_PRODUCT_EXPORT" /D "_USE_VTBL" /D "_USE_KCA" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_remote.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_remote.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=advapi32.lib Rpcrt4.lib kltrace.lib klcsc.lib klsecur.lib FSSync.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\pr_remote.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_remote.dll" /implib:"../Release/pr_remote.lib" /pdbtype:sept /libpath:"..\..\CS AdminKit\DEVELOPMENT\NULIB" /libpath:"..\release" /libpath:"..\..\CommonFiles\ReleaseDll" 
LINK32_OBJS= \
	"$(INTDIR)\pr_system_c.obj" \
	"$(INTDIR)\pr_system_s.obj" \
	"$(INTDIR)\rpc_system_c.obj" \
	"$(INTDIR)\rpc_system_s.obj" \
	"$(INTDIR)\bl_component.obj" \
	"$(INTDIR)\pr_internal.obj" \
	"$(INTDIR)\pr_library.obj" \
	"$(INTDIR)\pr_remote.obj" \
	"$(INTDIR)\pr_root_c.obj" \
	"$(INTDIR)\pr_root_s.obj" \
	"$(INTDIR)\rpc_connect.obj" \
	"$(INTDIR)\rpc_remote_c.obj" \
	"$(INTDIR)\rpc_remote_s.obj" \
	"$(INTDIR)\rpc_root_c.obj" \
	"$(INTDIR)\rpc_root_s.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\SharedTable.obj" \
	"$(INTDIR)\pr_remote.res"

"$(OUTDIR)\pr_remote.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=\out\Release MBCS
TargetPath=\out\Release MBCS\pr_remote.dll
TargetName=pr_remote
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release MBCS
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\pr_remote.dll"
   tsigner "\out\Release MBCS\pr_remote.dll"
	addsym \out\Release MBCS\pr_remote.pdb pr_remote
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
!IF EXISTS("pr_remote.dep")
!INCLUDE "pr_remote.dep"
!ELSE 
!MESSAGE Warning: cannot find "pr_remote.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pr_remote - Win32 Release" || "$(CFG)" == "pr_remote - Win32 Debug" || "$(CFG)" == "pr_remote - Win32 Debug MBCS" || "$(CFG)" == "pr_remote - Win32 Release MBCS"
SOURCE=.\pr_system\pr_system_c.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_system_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_system_c.obj"	"$(INTDIR)\pr_system_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_system_c.obj"	"$(INTDIR)\pr_system_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_system_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pr_system\pr_system_s.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_system_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_system_s.obj"	"$(INTDIR)\pr_system_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_system_s.obj"	"$(INTDIR)\pr_system_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_system_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pr_system\rpc_system_c.c

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_system_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_system_c.obj"	"$(INTDIR)\rpc_system_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_system_c.obj"	"$(INTDIR)\rpc_system_c.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_system_c.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\pr_system\rpc_system_s.c

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_system_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_system_s.obj"	"$(INTDIR)\rpc_system_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_system_s.obj"	"$(INTDIR)\rpc_system_s.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_system_s.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\bl_component.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\bl_component.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\bl_component.obj"	"$(INTDIR)\bl_component.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\bl_component.obj"	"$(INTDIR)\bl_component.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\bl_component.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pr_internal.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_internal.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_internal.obj"	"$(INTDIR)\pr_internal.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_internal.obj"	"$(INTDIR)\pr_internal.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_internal.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pr_library.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_library.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_library.obj"	"$(INTDIR)\pr_library.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_library.obj"	"$(INTDIR)\pr_library.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_library.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pr_remote.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_remote.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_remote.obj"	"$(INTDIR)\pr_remote.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_remote.obj"	"$(INTDIR)\pr_remote.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_remote.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pr_remote.rc

"$(INTDIR)\pr_remote.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\pr_root_c.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_root_c.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_root_c.obj"	"$(INTDIR)\pr_root_c.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_root_c.obj"	"$(INTDIR)\pr_root_c.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_root_c.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\pr_root_s.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\pr_root_s.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\pr_root_s.obj"	"$(INTDIR)\pr_root_s.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\pr_root_s.obj"	"$(INTDIR)\pr_root_s.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\pr_root_s.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rpc_connect.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_connect.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_connect.obj"	"$(INTDIR)\rpc_connect.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_connect.obj"	"$(INTDIR)\rpc_connect.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_connect.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\idl\rpc_remote.idl

!IF  "$(CFG)" == "pr_remote - Win32 Release"

InputPath=.\idl\rpc_remote.idl
InputName=rpc_remote

".\rpc_remote_c.c"	".\rpc_remote_s.c"	".\rpc_remote.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include
<< 
	

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

InputPath=.\idl\rpc_remote.idl
InputName=rpc_remote

".\rpc_remote_c.c"	".\rpc_remote_s.c"	".\rpc_remote.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../include @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include
<< 
	

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

!ENDIF 

SOURCE=.\rpc_remote_c.c

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_remote_c.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_remote_c.obj"	"$(INTDIR)\rpc_remote_c.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_remote_c.obj"	"$(INTDIR)\rpc_remote_c.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_remote_c.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rpc_remote_s.c

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_remote_s.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_remote_s.obj"	"$(INTDIR)\rpc_remote_s.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_remote_s.obj"	"$(INTDIR)\rpc_remote_s.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_remote_s.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\idl\rpc_root.idl

!IF  "$(CFG)" == "pr_remote - Win32 Release"

InputPath=.\idl\rpc_root.idl
InputName=rpc_root

".\rpc_root_c.c"	".\rpc_root_s.c"	".\rpc_root.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include
<< 
	

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

InputPath=.\idl\rpc_root.idl
InputName=rpc_root

".\rpc_root_c.c"	".\rpc_root_s.c"	".\rpc_root.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../include @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -I ../include/iface -I ../include
<< 
	

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

!ENDIF 

SOURCE=.\rpc_root_c.c

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_root_c.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_root_c.obj"	"$(INTDIR)\rpc_root_c.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_root_c.obj"	"$(INTDIR)\rpc_root_c.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_root_c.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rpc_root_s.c

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\rpc_root_s.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\rpc_root_s.obj"	"$(INTDIR)\rpc_root_s.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\rpc_root_s.obj"	"$(INTDIR)\rpc_root_s.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\rpc_root_s.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\idl\rpc_system.idl

!IF  "$(CFG)" == "pr_remote - Win32 Release"

InputPath=.\idl\rpc_system.idl
InputName=rpc_system

".\pr_system\rpc_system_c.c"	".\pr_system\rpc_system_s.c"	".\pr_system\rpc_system.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -out ./pr_system -I ../include/iface
<< 
	

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"

InputPath=.\idl\rpc_system.idl
InputName=rpc_system

".\pr_system\rpc_system_c.c"	".\pr_system\rpc_system_s.c"	".\pr_system\rpc_system.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../include @./idl/rpc.rsp -prefix server PR $(InputPath) -acf ./idl/$(InputName).acf -out ./pr_system -I ../include/iface
<< 
	

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"

!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"

!ENDIF 

SOURCE=..\..\CommonFiles\Service\sa.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\sa.obj"	"$(INTDIR)\sa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\sa.obj"	"$(INTDIR)\sa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\SharedTable.cpp

!IF  "$(CFG)" == "pr_remote - Win32 Release"


"$(INTDIR)\SharedTable.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug"


"$(INTDIR)\SharedTable.obj"	"$(INTDIR)\SharedTable.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Debug MBCS"


"$(INTDIR)\SharedTable.obj"	"$(INTDIR)\SharedTable.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "pr_remote - Win32 Release MBCS"


"$(INTDIR)\SharedTable.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


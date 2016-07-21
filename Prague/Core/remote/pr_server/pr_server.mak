# Microsoft Developer Studio Generated NMAKE File, Based on pr_server.dsp
!IF "$(CFG)" == ""
CFG=pr_server - Win32 Debug
!MESSAGE No configuration specified. Defaulting to pr_server - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pr_server - Win32 Release" && "$(CFG)" != "pr_server - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pr_server.mak" CFG="pr_server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pr_server - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_server - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pr_server - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/remote/pr_server
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : ".\rpc_string_s.c" ".\rpc_string.h" ".\rpc_reg_s.c" ".\rpc_reg.h" ".\rpc_os_s.c" ".\rpc_os.h" ".\rpc_objptr_s.c" ".\rpc_objptr.h" ".\rpc_mchk_s.c" ".\rpc_mchk.h" ".\rpc_io_s.c" ".\rpc_io.h" ".\rpc_insider_s.c" ".\rpc_insider.h" ".\rpc_ifenum_s.c" ".\rpc_ifenum.h" ".\rpc_engine_s.c" ".\rpc_engine.h" ".\rpc_buffer_s.c" ".\rpc_buffer.h" "$(OUTDIR)\pr_server.ppl"


CLEAN :
	-@erase "$(INTDIR)\buffer_s.obj"
	-@erase "$(INTDIR)\engine_s.obj"
	-@erase "$(INTDIR)\ifenum_s.obj"
	-@erase "$(INTDIR)\insider_s.obj"
	-@erase "$(INTDIR)\io_s.obj"
	-@erase "$(INTDIR)\mchk_s.obj"
	-@erase "$(INTDIR)\objptr_s.obj"
	-@erase "$(INTDIR)\os_s.obj"
	-@erase "$(INTDIR)\pr_server.obj"
	-@erase "$(INTDIR)\pr_server.res"
	-@erase "$(INTDIR)\reg_s.obj"
	-@erase "$(INTDIR)\rpc_buffer_s.obj"
	-@erase "$(INTDIR)\rpc_engine_s.obj"
	-@erase "$(INTDIR)\rpc_ifenum_s.obj"
	-@erase "$(INTDIR)\rpc_insider_s.obj"
	-@erase "$(INTDIR)\rpc_io_s.obj"
	-@erase "$(INTDIR)\rpc_mchk_s.obj"
	-@erase "$(INTDIR)\rpc_objptr_s.obj"
	-@erase "$(INTDIR)\rpc_os_s.obj"
	-@erase "$(INTDIR)\rpc_reg_s.obj"
	-@erase "$(INTDIR)\rpc_string_s.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string_s.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_server.pdb"
	-@erase "$(OUTDIR)\pr_server.ppl"
	-@erase "..\..\Release\pr_server.exp"
	-@erase "rpc_buffer.h"
	-@erase "rpc_buffer_s.c"
	-@erase "rpc_engine.h"
	-@erase "rpc_engine_s.c"
	-@erase "rpc_ifenum.h"
	-@erase "rpc_ifenum_s.c"
	-@erase "rpc_insider.h"
	-@erase "rpc_insider_s.c"
	-@erase "rpc_io.h"
	-@erase "rpc_io_s.c"
	-@erase "rpc_mchk.h"
	-@erase "rpc_mchk_s.c"
	-@erase "rpc_objptr.h"
	-@erase "rpc_objptr_s.c"
	-@erase "rpc_os.h"
	-@erase "rpc_os_s.c"
	-@erase "rpc_reg.h"
	-@erase "rpc_reg_s.c"
	-@erase "rpc_string.h"
	-@erase "rpc_string_s.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GR /GX /Zi /O2 /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pr_server_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_server.res" /i "..\..\..\commonfiles" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_server.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=rpcrt4.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\pr_server.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_server.ppl" /implib:"../../Release/pr_server.lib" /pdbtype:sept /opt:ref /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\buffer_s.obj" \
	"$(INTDIR)\engine_s.obj" \
	"$(INTDIR)\ifenum_s.obj" \
	"$(INTDIR)\insider_s.obj" \
	"$(INTDIR)\io_s.obj" \
	"$(INTDIR)\mchk_s.obj" \
	"$(INTDIR)\objptr_s.obj" \
	"$(INTDIR)\os_s.obj" \
	"$(INTDIR)\pr_server.obj" \
	"$(INTDIR)\reg_s.obj" \
	"$(INTDIR)\rpc_buffer_s.obj" \
	"$(INTDIR)\rpc_engine_s.obj" \
	"$(INTDIR)\rpc_ifenum_s.obj" \
	"$(INTDIR)\rpc_insider_s.obj" \
	"$(INTDIR)\rpc_io_s.obj" \
	"$(INTDIR)\rpc_mchk_s.obj" \
	"$(INTDIR)\rpc_objptr_s.obj" \
	"$(INTDIR)\rpc_os_s.obj" \
	"$(INTDIR)\rpc_reg_s.obj" \
	"$(INTDIR)\rpc_string_s.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string_s.obj" \
	"$(INTDIR)\pr_server.res"

"$(OUTDIR)\pr_server.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\pr_server.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : ".\rpc_string_s.c" ".\rpc_string.h" ".\rpc_reg_s.c" ".\rpc_reg.h" ".\rpc_os_s.c" ".\rpc_os.h" ".\rpc_objptr_s.c" ".\rpc_objptr.h" ".\rpc_mchk_s.c" ".\rpc_mchk.h" ".\rpc_io_s.c" ".\rpc_io.h" ".\rpc_insider_s.c" ".\rpc_insider.h" ".\rpc_ifenum_s.c" ".\rpc_ifenum.h" ".\rpc_engine_s.c" ".\rpc_engine.h" ".\rpc_buffer_s.c" ".\rpc_buffer.h" "$(OUTDIR)\pr_server.ppl"
   tsigner "\out\Release\pr_server.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/remote/pr_server
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : ".\rpc_string_s.c" ".\rpc_string.h" ".\rpc_reg_s.c" ".\rpc_reg.h" ".\rpc_os_s.c" ".\rpc_os.h" ".\rpc_objptr_s.c" ".\rpc_objptr.h" ".\rpc_mchk_s.c" ".\rpc_mchk.h" ".\rpc_io_s.c" ".\rpc_io.h" ".\rpc_insider_s.c" ".\rpc_insider.h" ".\rpc_ifenum_s.c" ".\rpc_ifenum.h" ".\rpc_engine_s.c" ".\rpc_engine.h" ".\rpc_buffer_s.c" ".\rpc_buffer.h" "$(OUTDIR)\pr_server.ppl"


CLEAN :
	-@erase "$(INTDIR)\buffer_s.obj"
	-@erase "$(INTDIR)\engine_s.obj"
	-@erase "$(INTDIR)\ifenum_s.obj"
	-@erase "$(INTDIR)\insider_s.obj"
	-@erase "$(INTDIR)\io_s.obj"
	-@erase "$(INTDIR)\mchk_s.obj"
	-@erase "$(INTDIR)\objptr_s.obj"
	-@erase "$(INTDIR)\os_s.obj"
	-@erase "$(INTDIR)\pr_server.obj"
	-@erase "$(INTDIR)\pr_server.res"
	-@erase "$(INTDIR)\reg_s.obj"
	-@erase "$(INTDIR)\rpc_buffer_s.obj"
	-@erase "$(INTDIR)\rpc_engine_s.obj"
	-@erase "$(INTDIR)\rpc_ifenum_s.obj"
	-@erase "$(INTDIR)\rpc_insider_s.obj"
	-@erase "$(INTDIR)\rpc_io_s.obj"
	-@erase "$(INTDIR)\rpc_mchk_s.obj"
	-@erase "$(INTDIR)\rpc_objptr_s.obj"
	-@erase "$(INTDIR)\rpc_os_s.obj"
	-@erase "$(INTDIR)\rpc_reg_s.obj"
	-@erase "$(INTDIR)\rpc_string_s.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string_s.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_server.ilk"
	-@erase "$(OUTDIR)\pr_server.pdb"
	-@erase "$(OUTDIR)\pr_server.ppl"
	-@erase "..\..\Debug\pr_server.exp"
	-@erase "rpc_buffer.h"
	-@erase "rpc_buffer_s.c"
	-@erase "rpc_engine.h"
	-@erase "rpc_engine_s.c"
	-@erase "rpc_ifenum.h"
	-@erase "rpc_ifenum_s.c"
	-@erase "rpc_insider.h"
	-@erase "rpc_insider_s.c"
	-@erase "rpc_io.h"
	-@erase "rpc_io_s.c"
	-@erase "rpc_mchk.h"
	-@erase "rpc_mchk_s.c"
	-@erase "rpc_objptr.h"
	-@erase "rpc_objptr_s.c"
	-@erase "rpc_os.h"
	-@erase "rpc_os_s.c"
	-@erase "rpc_reg.h"
	-@erase "rpc_reg_s.c"
	-@erase "rpc_string.h"
	-@erase "rpc_string_s.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "pr_server_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_server.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_server.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=rpcrt4.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\pr_server.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_server.ppl" /implib:"../../Debug/pr_server.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\buffer_s.obj" \
	"$(INTDIR)\engine_s.obj" \
	"$(INTDIR)\ifenum_s.obj" \
	"$(INTDIR)\insider_s.obj" \
	"$(INTDIR)\io_s.obj" \
	"$(INTDIR)\mchk_s.obj" \
	"$(INTDIR)\objptr_s.obj" \
	"$(INTDIR)\os_s.obj" \
	"$(INTDIR)\pr_server.obj" \
	"$(INTDIR)\reg_s.obj" \
	"$(INTDIR)\rpc_buffer_s.obj" \
	"$(INTDIR)\rpc_engine_s.obj" \
	"$(INTDIR)\rpc_ifenum_s.obj" \
	"$(INTDIR)\rpc_insider_s.obj" \
	"$(INTDIR)\rpc_io_s.obj" \
	"$(INTDIR)\rpc_mchk_s.obj" \
	"$(INTDIR)\rpc_objptr_s.obj" \
	"$(INTDIR)\rpc_os_s.obj" \
	"$(INTDIR)\rpc_reg_s.obj" \
	"$(INTDIR)\rpc_string_s.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string_s.obj" \
	"$(INTDIR)\pr_server.res"

"$(OUTDIR)\pr_server.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("pr_server.dep")
!INCLUDE "pr_server.dep"
!ELSE 
!MESSAGE Warning: cannot find "pr_server.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pr_server - Win32 Release" || "$(CFG)" == "pr_server - Win32 Debug"
SOURCE=.\buffer_s.cpp

"$(INTDIR)\buffer_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\engine_s.cpp

"$(INTDIR)\engine_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ifenum_s.cpp

"$(INTDIR)\ifenum_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\insider_s.cpp

"$(INTDIR)\insider_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\io_s.cpp

"$(INTDIR)\io_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mchk_s.cpp

"$(INTDIR)\mchk_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objptr_s.cpp

"$(INTDIR)\objptr_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os_s.cpp

"$(INTDIR)\os_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pr_server.cpp

"$(INTDIR)\pr_server.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pr_server.rc

!IF  "$(CFG)" == "pr_server - Win32 Release"


"$(INTDIR)\pr_server.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\pr_server.res" /i "..\..\..\commonfiles" /i "..\..\..\CommonFiles" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"


"$(INTDIR)\pr_server.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\pr_server.res" /i "..\..\..\CommonFiles" /i "../../../commonfiles" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\reg_s.cpp

"$(INTDIR)\reg_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_buffer.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_buffer.idl
InputName=rpc_buffer

".\rpc_buffer_s.c"	".\rpc_buffer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_buffer.idl
InputName=rpc_buffer

".\rpc_buffer_s.c"	".\rpc_buffer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_buffer_s.c

"$(INTDIR)\rpc_buffer_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_engine.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_engine.idl
InputName=rpc_engine

".\rpc_engine_s.c"	".\rpc_engine.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_engine.idl
InputName=rpc_engine

".\rpc_engine_s.c"	".\rpc_engine.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_engine_s.c

"$(INTDIR)\rpc_engine_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_ifenum.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_ifenum.idl
InputName=rpc_ifenum

".\rpc_ifenum_s.c"	".\rpc_ifenum.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_ifenum.idl
InputName=rpc_ifenum

".\rpc_ifenum_s.c"	".\rpc_ifenum.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_ifenum_s.c

"$(INTDIR)\rpc_ifenum_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_insider.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_insider.idl
InputName=rpc_insider

".\rpc_insider_s.c"	".\rpc_insider.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_insider.idl
InputName=rpc_insider

".\rpc_insider_s.c"	".\rpc_insider.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_insider_s.c

"$(INTDIR)\rpc_insider_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_io.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_io.idl
InputName=rpc_io

".\rpc_io_s.c"	".\rpc_io.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_io.idl
InputName=rpc_io

".\rpc_io_s.c"	".\rpc_io.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_io_s.c

"$(INTDIR)\rpc_io_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_mchk.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_mchk.idl
InputName=rpc_mchk

".\rpc_mchk_s.c"	".\rpc_mchk.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_mchk.idl
InputName=rpc_mchk

".\rpc_mchk_s.c"	".\rpc_mchk.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_mchk_s.c

"$(INTDIR)\rpc_mchk_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_objptr.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_objptr.idl
InputName=rpc_objptr

".\rpc_objptr_s.c"	".\rpc_objptr.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_objptr.idl
InputName=rpc_objptr

".\rpc_objptr_s.c"	".\rpc_objptr.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_objptr_s.c

"$(INTDIR)\rpc_objptr_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_os.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_os.idl
InputName=rpc_os

".\rpc_os_s.c"	".\rpc_os.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_os.idl
InputName=rpc_os

".\rpc_os_s.c"	".\rpc_os.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_os_s.c

"$(INTDIR)\rpc_os_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_reg.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_reg.idl
InputName=rpc_reg

".\rpc_reg_s.c"	".\rpc_reg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_reg.idl
InputName=rpc_reg

".\rpc_reg_s.c"	".\rpc_reg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_reg_s.c

"$(INTDIR)\rpc_reg_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_string.idl

!IF  "$(CFG)" == "pr_server - Win32 Release"

InputPath=..\idl\rpc_string.idl
InputName=rpc_string

".\rpc_string_s.c"	".\rpc_string.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_server - Win32 Debug"

InputPath=..\idl\rpc_string.idl
InputName=rpc_string

".\rpc_string_s.c"	".\rpc_string.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -client none -prefix server PR $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_string_s.c

"$(INTDIR)\rpc_string_s.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\string_s.cpp

"$(INTDIR)\string_s.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on pr_client.dsp
!IF "$(CFG)" == ""
CFG=pr_client - Win32 Debug
!MESSAGE No configuration specified. Defaulting to pr_client - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pr_client - Win32 Release" && "$(CFG)" != "pr_client - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pr_client.mak" CFG="pr_client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pr_client - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pr_client - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "pr_client - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/remote/pr_client
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : ".\rpc_string_c.c" ".\rpc_string.h" ".\rpc_reg_c.c" ".\rpc_reg.h" ".\rpc_os_c.c" ".\rpc_os.h" ".\rpc_objptr_c.c" ".\rpc_objptr.h" ".\rpc_mchk_c.c" ".\rpc_mchk.h" ".\rpc_io_c.c" ".\rpc_io.h" ".\rpc_insider_c.c" ".\rpc_insider.h" ".\rpc_ifenum_c.c" ".\rpc_ifenum.h" ".\rpc_engine_c.c" ".\rpc_engine.h" ".\rpc_buffer_c.c" ".\rpc_buffer.h" "$(OUTDIR)\pr_client.ppl" "..\..\..\temp\Release\prague\remote\pr_client\pr_client.pch"


CLEAN :
	-@erase "$(INTDIR)\buffer_c.obj"
	-@erase "$(INTDIR)\engine_c.obj"
	-@erase "$(INTDIR)\ifenum_c.obj"
	-@erase "$(INTDIR)\insider_c.obj"
	-@erase "$(INTDIR)\io_c.obj"
	-@erase "$(INTDIR)\mchk_c.obj"
	-@erase "$(INTDIR)\objptr_c.obj"
	-@erase "$(INTDIR)\os_c.obj"
	-@erase "$(INTDIR)\pr_client.obj"
	-@erase "$(INTDIR)\pr_client.pch"
	-@erase "$(INTDIR)\pr_client.res"
	-@erase "$(INTDIR)\reg_c.obj"
	-@erase "$(INTDIR)\rpc_buffer_c.obj"
	-@erase "$(INTDIR)\rpc_engine_c.obj"
	-@erase "$(INTDIR)\rpc_ifenum_c.obj"
	-@erase "$(INTDIR)\rpc_insider_c.obj"
	-@erase "$(INTDIR)\rpc_io_c.obj"
	-@erase "$(INTDIR)\rpc_mchk_c.obj"
	-@erase "$(INTDIR)\rpc_objptr_c.obj"
	-@erase "$(INTDIR)\rpc_os_c.obj"
	-@erase "$(INTDIR)\rpc_reg_c.obj"
	-@erase "$(INTDIR)\rpc_string_c.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string_c.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_client.pdb"
	-@erase "$(OUTDIR)\pr_client.ppl"
	-@erase "..\..\Release\pr_client.exp"
	-@erase "rpc_buffer.h"
	-@erase "rpc_buffer_c.c"
	-@erase "rpc_engine.h"
	-@erase "rpc_engine_c.c"
	-@erase "rpc_ifenum.h"
	-@erase "rpc_ifenum_c.c"
	-@erase "rpc_insider.h"
	-@erase "rpc_insider_c.c"
	-@erase "rpc_io.h"
	-@erase "rpc_io_c.c"
	-@erase "rpc_mchk.h"
	-@erase "rpc_mchk_c.c"
	-@erase "rpc_objptr.h"
	-@erase "rpc_objptr_c.c"
	-@erase "rpc_os.h"
	-@erase "rpc_os_c.c"
	-@erase "rpc_reg.h"
	-@erase "rpc_reg_c.c"
	-@erase "rpc_string.h"
	-@erase "rpc_string_c.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GR /GX /Zi /O2 /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_client.res" /i "..\..\..\commonfiles" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_client.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=rpcrt4.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\pr_client.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_client.ppl" /implib:"../../Release/pr_client.lib" /pdbtype:sept /opt:ref /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\buffer_c.obj" \
	"$(INTDIR)\engine_c.obj" \
	"$(INTDIR)\ifenum_c.obj" \
	"$(INTDIR)\insider_c.obj" \
	"$(INTDIR)\io_c.obj" \
	"$(INTDIR)\mchk_c.obj" \
	"$(INTDIR)\objptr_c.obj" \
	"$(INTDIR)\os_c.obj" \
	"$(INTDIR)\pr_client.obj" \
	"$(INTDIR)\reg_c.obj" \
	"$(INTDIR)\rpc_buffer_c.obj" \
	"$(INTDIR)\rpc_engine_c.obj" \
	"$(INTDIR)\rpc_ifenum_c.obj" \
	"$(INTDIR)\rpc_insider_c.obj" \
	"$(INTDIR)\rpc_io_c.obj" \
	"$(INTDIR)\rpc_mchk_c.obj" \
	"$(INTDIR)\rpc_objptr_c.obj" \
	"$(INTDIR)\rpc_os_c.obj" \
	"$(INTDIR)\rpc_reg_c.obj" \
	"$(INTDIR)\rpc_string_c.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string_c.obj" \
	"$(INTDIR)\pr_client.res"

"$(OUTDIR)\pr_client.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\pr_client.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : ".\rpc_string_c.c" ".\rpc_string.h" ".\rpc_reg_c.c" ".\rpc_reg.h" ".\rpc_os_c.c" ".\rpc_os.h" ".\rpc_objptr_c.c" ".\rpc_objptr.h" ".\rpc_mchk_c.c" ".\rpc_mchk.h" ".\rpc_io_c.c" ".\rpc_io.h" ".\rpc_insider_c.c" ".\rpc_insider.h" ".\rpc_ifenum_c.c" ".\rpc_ifenum.h" ".\rpc_engine_c.c" ".\rpc_engine.h" ".\rpc_buffer_c.c" ".\rpc_buffer.h" "$(OUTDIR)\pr_client.ppl" "..\..\..\temp\Release\prague\remote\pr_client\pr_client.pch"
   tsigner "\out\Release\pr_client.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/remote/pr_client
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : ".\rpc_string_c.c" ".\rpc_string.h" ".\rpc_reg_c.c" ".\rpc_reg.h" ".\rpc_os_c.c" ".\rpc_os.h" ".\rpc_objptr_c.c" ".\rpc_objptr.h" ".\rpc_mchk_c.c" ".\rpc_mchk.h" ".\rpc_io_c.c" ".\rpc_io.h" ".\rpc_insider_c.c" ".\rpc_insider.h" ".\rpc_ifenum_c.c" ".\rpc_ifenum.h" ".\rpc_engine_c.c" ".\rpc_engine.h" ".\rpc_buffer_c.c" ".\rpc_buffer.h" "$(OUTDIR)\pr_client.ppl" "..\..\..\temp\Debug\prague\remote\pr_client\pr_client.pch"


CLEAN :
	-@erase "$(INTDIR)\buffer_c.obj"
	-@erase "$(INTDIR)\engine_c.obj"
	-@erase "$(INTDIR)\ifenum_c.obj"
	-@erase "$(INTDIR)\insider_c.obj"
	-@erase "$(INTDIR)\io_c.obj"
	-@erase "$(INTDIR)\mchk_c.obj"
	-@erase "$(INTDIR)\objptr_c.obj"
	-@erase "$(INTDIR)\os_c.obj"
	-@erase "$(INTDIR)\pr_client.obj"
	-@erase "$(INTDIR)\pr_client.pch"
	-@erase "$(INTDIR)\pr_client.res"
	-@erase "$(INTDIR)\reg_c.obj"
	-@erase "$(INTDIR)\rpc_buffer_c.obj"
	-@erase "$(INTDIR)\rpc_engine_c.obj"
	-@erase "$(INTDIR)\rpc_ifenum_c.obj"
	-@erase "$(INTDIR)\rpc_insider_c.obj"
	-@erase "$(INTDIR)\rpc_io_c.obj"
	-@erase "$(INTDIR)\rpc_mchk_c.obj"
	-@erase "$(INTDIR)\rpc_objptr_c.obj"
	-@erase "$(INTDIR)\rpc_os_c.obj"
	-@erase "$(INTDIR)\rpc_reg_c.obj"
	-@erase "$(INTDIR)\rpc_string_c.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string_c.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\pr_client.ilk"
	-@erase "$(OUTDIR)\pr_client.pdb"
	-@erase "$(OUTDIR)\pr_client.ppl"
	-@erase "..\..\Debug\pr_client.exp"
	-@erase "rpc_buffer.h"
	-@erase "rpc_buffer_c.c"
	-@erase "rpc_engine.h"
	-@erase "rpc_engine_c.c"
	-@erase "rpc_ifenum.h"
	-@erase "rpc_ifenum_c.c"
	-@erase "rpc_insider.h"
	-@erase "rpc_insider_c.c"
	-@erase "rpc_io.h"
	-@erase "rpc_io_c.c"
	-@erase "rpc_mchk.h"
	-@erase "rpc_mchk_c.c"
	-@erase "rpc_objptr.h"
	-@erase "rpc_objptr_c.c"
	-@erase "rpc_os.h"
	-@erase "rpc_os_c.c"
	-@erase "rpc_reg.h"
	-@erase "rpc_reg_c.c"
	-@erase "rpc_string.h"
	-@erase "rpc_string_c.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\pr_client.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pr_client.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=rpcrt4.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\pr_client.pdb" /debug /machine:I386 /out:"$(OUTDIR)\pr_client.ppl" /implib:"../../Debug/pr_client.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\buffer_c.obj" \
	"$(INTDIR)\engine_c.obj" \
	"$(INTDIR)\ifenum_c.obj" \
	"$(INTDIR)\insider_c.obj" \
	"$(INTDIR)\io_c.obj" \
	"$(INTDIR)\mchk_c.obj" \
	"$(INTDIR)\objptr_c.obj" \
	"$(INTDIR)\os_c.obj" \
	"$(INTDIR)\pr_client.obj" \
	"$(INTDIR)\reg_c.obj" \
	"$(INTDIR)\rpc_buffer_c.obj" \
	"$(INTDIR)\rpc_engine_c.obj" \
	"$(INTDIR)\rpc_ifenum_c.obj" \
	"$(INTDIR)\rpc_insider_c.obj" \
	"$(INTDIR)\rpc_io_c.obj" \
	"$(INTDIR)\rpc_mchk_c.obj" \
	"$(INTDIR)\rpc_objptr_c.obj" \
	"$(INTDIR)\rpc_os_c.obj" \
	"$(INTDIR)\rpc_reg_c.obj" \
	"$(INTDIR)\rpc_string_c.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string_c.obj" \
	"$(INTDIR)\pr_client.res"

"$(OUTDIR)\pr_client.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("pr_client.dep")
!INCLUDE "pr_client.dep"
!ELSE 
!MESSAGE Warning: cannot find "pr_client.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pr_client - Win32 Release" || "$(CFG)" == "pr_client - Win32 Debug"
SOURCE=.\buffer_c.cpp

"$(INTDIR)\buffer_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\engine_c.cpp

"$(INTDIR)\engine_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ifenum_c.cpp

"$(INTDIR)\ifenum_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\insider_c.cpp

"$(INTDIR)\insider_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\io_c.cpp

"$(INTDIR)\io_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mchk_c.cpp

"$(INTDIR)\mchk_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objptr_c.cpp

"$(INTDIR)\objptr_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os_c.cpp

"$(INTDIR)\os_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pr_client.cpp

"$(INTDIR)\pr_client.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pr_client.rc

!IF  "$(CFG)" == "pr_client - Win32 Release"


"$(INTDIR)\pr_client.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x409 /fo"$(INTDIR)\pr_client.res" /i "..\..\..\commonfiles" /i "..\..\..\CommonFiles" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"


"$(INTDIR)\pr_client.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\pr_client.res" /i "..\..\..\CommonFiles" /i "../../../commonfiles" /d "_DEBUG" $(SOURCE)


!ENDIF 

SOURCE=.\reg_c.cpp

"$(INTDIR)\reg_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_buffer.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_buffer.idl
InputName=rpc_buffer

".\rpc_buffer_c.c"	".\rpc_buffer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_buffer.idl
InputName=rpc_buffer

".\rpc_buffer_c.c"	".\rpc_buffer.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_buffer_c.c

"$(INTDIR)\rpc_buffer_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_engine.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_engine.idl
InputName=rpc_engine

".\rpc_engine_c.c"	".\rpc_engine.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_engine.idl
InputName=rpc_engine
USERDEP__RPC_E="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_engine_c.c"	".\rpc_engine.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_E)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_engine_c.c

"$(INTDIR)\rpc_engine_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_ifenum.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_ifenum.idl
InputName=rpc_ifenum

".\rpc_ifenum_c.c"	".\rpc_ifenum.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_ifenum.idl
InputName=rpc_ifenum
USERDEP__RPC_I="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_ifenum_c.c"	".\rpc_ifenum.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_I)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_ifenum_c.c

"$(INTDIR)\rpc_ifenum_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_insider.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_insider.idl
InputName=rpc_insider

".\rpc_insider_c.c"	".\rpc_insider.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_insider.idl
InputName=rpc_insider

".\rpc_insider_c.c"	".\rpc_insider.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_insider_c.c

"$(INTDIR)\rpc_insider_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_io.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_io.idl
InputName=rpc_io

".\rpc_io_c.c"	".\rpc_io.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_io.idl
InputName=rpc_io
USERDEP__RPC_IO="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_io_c.c"	".\rpc_io.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_IO)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_io_c.c

"$(INTDIR)\rpc_io_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_mchk.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_mchk.idl
InputName=rpc_mchk

".\rpc_mchk_c.c"	".\rpc_mchk.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_mchk.idl
InputName=rpc_mchk

".\rpc_mchk_c.c"	".\rpc_mchk.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_mchk_c.c

"$(INTDIR)\rpc_mchk_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_objptr.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_objptr.idl
InputName=rpc_objptr

".\rpc_objptr_c.c"	".\rpc_objptr.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_objptr.idl
InputName=rpc_objptr
USERDEP__RPC_O="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_objptr_c.c"	".\rpc_objptr.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_O)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_objptr_c.c

"$(INTDIR)\rpc_objptr_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_os.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_os.idl
InputName=rpc_os

".\rpc_os_c.c"	".\rpc_os.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_os.idl
InputName=rpc_os
USERDEP__RPC_OS="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_os_c.c"	".\rpc_os.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_OS)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_os_c.c

"$(INTDIR)\rpc_os_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_reg.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_reg.idl
InputName=rpc_reg

".\rpc_reg_c.c"	".\rpc_reg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_reg.idl
InputName=rpc_reg
USERDEP__RPC_R="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_reg_c.c"	".\rpc_reg.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_R)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_reg_c.c

"$(INTDIR)\rpc_reg_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\idl\rpc_string.idl

!IF  "$(CFG)" == "pr_client - Win32 Release"

InputPath=..\idl\rpc_string.idl
InputName=rpc_string

".\rpc_string_c.c"	".\rpc_string.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	midl @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

InputPath=..\idl\rpc_string.idl
InputName=rpc_string
USERDEP__RPC_S="../idl/rpc.rsp"	"../idl/$(InputName).acf"	

".\rpc_string_c.c"	".\rpc_string.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__RPC_S)
	<<tempfile.bat 
	@echo off 
	midl -I ../../include @../idl/rpc.rsp -server none $(InputPath) -acf ../idl/$(InputName).acf
<< 
	

!ENDIF 

SOURCE=.\rpc_string_c.c

"$(INTDIR)\rpc_string_c.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "pr_client - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GR /GX /Zi /O2 /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /Fp"$(INTDIR)\pr_client.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\pr_client.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "pr_client - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "../../include" /I "../../include/iface" /I "../../../commonfiles" /I "../idl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PR_CLIENT_EXPORTS" /Fp"$(INTDIR)\pr_client.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\pr_client.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\string_c.cpp

"$(INTDIR)\string_c.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on SWManager.dsp
!IF "$(CFG)" == ""
CFG=SWManager - Win32 Unicode Debug
!MESSAGE No configuration specified. Defaulting to SWManager - Win32 Unicode Debug.
!ENDIF 

!IF "$(CFG)" != "SWManager - Win32 Release" && "$(CFG)" != "SWManager - Win32 Debug" && "$(CFG)" != "SWManager - Win32 DebugDll" && "$(CFG)" != "SWManager - Win32 ReleaseDll" && "$(CFG)" != "SWManager - Win32 Unicode DebugDll" && "$(CFG)" != "SWManager - Win32 Unicode Release" && "$(CFG)" != "SWManager - Win32 Unicode Debug" && "$(CFG)" != "SWManager - Win32 Unicode ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SWManager.mak" CFG="SWManager - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SWManager - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SWManager - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "SWManager - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\CommonFiles\Release\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\Release\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Release\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\Release\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\CommonFiles\Debug\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\Debug\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\Debug\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\Debug\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 DebugDll"

OUTDIR=.\DebugDll
INTDIR=.\DebugDll

ALL : "..\CommonFiles\DebugDll\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\DebugDll\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugDll\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\DebugDll\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 ReleaseDll"

OUTDIR=.\ReleaseDll
INTDIR=.\ReleaseDll

ALL : "..\CommonFiles\ReleaseDll\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseDll\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseDll\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\ReleaseDll\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode DebugDll"

OUTDIR=.\DebugUDll
INTDIR=.\DebugUDll

ALL : "..\CommonFiles\DebugUDll\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\DebugUDll\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugUDll\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\DebugUDll\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode Release"

OUTDIR=.\ReleaseU
INTDIR=.\ReleaseU

ALL : "..\CommonFiles\ReleaseU\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseU\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseU\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\ReleaseU\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU

ALL : "..\CommonFiles\DebugU\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\CommonFiles\DebugU\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\DebugU\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\DebugU\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "SWManager - Win32 Unicode ReleaseDll"

OUTDIR=.\ReleaseUDll
INTDIR=.\ReleaseUDll

ALL : "..\CommonFiles\ReleaseUDll\SWM.lib"


CLEAN :
	-@erase "$(INTDIR)\Checker.obj"
	-@erase "$(INTDIR)\CheckerR.obj"
	-@erase "$(INTDIR)\CheckerW.obj"
	-@erase "$(INTDIR)\Encrypt.obj"
	-@erase "$(INTDIR)\EncryptR.obj"
	-@erase "$(INTDIR)\EncryptW.obj"
	-@erase "$(INTDIR)\Manager.obj"
	-@erase "$(INTDIR)\ManagerR.obj"
	-@erase "$(INTDIR)\ManagerW.obj"
	-@erase "$(INTDIR)\Packer.obj"
	-@erase "$(INTDIR)\PackerR.obj"
	-@erase "$(INTDIR)\PackerW.obj"
	-@erase "$(INTDIR)\RawData.obj"
	-@erase "$(INTDIR)\RawDataR.obj"
	-@erase "$(INTDIR)\RawDataW.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\CommonFiles\ReleaseUDll\SWM.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SWManager.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\CommonFiles\ReleaseUDll\SWM.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Checker.obj" \
	"$(INTDIR)\CheckerR.obj" \
	"$(INTDIR)\CheckerW.obj" \
	"$(INTDIR)\Encrypt.obj" \
	"$(INTDIR)\EncryptR.obj" \
	"$(INTDIR)\EncryptW.obj" \
	"$(INTDIR)\Manager.obj" \
	"$(INTDIR)\ManagerR.obj" \
	"$(INTDIR)\ManagerW.obj" \
	"$(INTDIR)\Packer.obj" \
	"$(INTDIR)\PackerR.obj" \
	"$(INTDIR)\PackerW.obj" \
	"$(INTDIR)\RawData.obj" \
	"$(INTDIR)\RawDataR.obj" \
	"$(INTDIR)\RawDataW.obj"

"..\CommonFiles\ReleaseUDll\SWM.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("SWManager.dep")
!INCLUDE "SWManager.dep"
!ELSE 
!MESSAGE Warning: cannot find "SWManager.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SWManager - Win32 Release" || "$(CFG)" == "SWManager - Win32 Debug" || "$(CFG)" == "SWManager - Win32 DebugDll" || "$(CFG)" == "SWManager - Win32 ReleaseDll" || "$(CFG)" == "SWManager - Win32 Unicode DebugDll" || "$(CFG)" == "SWManager - Win32 Unicode Release" || "$(CFG)" == "SWManager - Win32 Unicode Debug" || "$(CFG)" == "SWManager - Win32 Unicode ReleaseDll"
SOURCE=.\Checker.c

"$(INTDIR)\Checker.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CheckerR.c

"$(INTDIR)\CheckerR.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CheckerW.c

"$(INTDIR)\CheckerW.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Encrypt.c

"$(INTDIR)\Encrypt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EncryptR.c

"$(INTDIR)\EncryptR.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EncryptW.c

"$(INTDIR)\EncryptW.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Manager.c

"$(INTDIR)\Manager.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ManagerR.c

"$(INTDIR)\ManagerR.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ManagerW.c

"$(INTDIR)\ManagerW.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Packer.c

"$(INTDIR)\Packer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PackerR.c

"$(INTDIR)\PackerR.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\PackerW.c

"$(INTDIR)\PackerW.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RawData.c

"$(INTDIR)\RawData.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RawDataR.c

"$(INTDIR)\RawDataR.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RawDataW.c

"$(INTDIR)\RawDataW.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


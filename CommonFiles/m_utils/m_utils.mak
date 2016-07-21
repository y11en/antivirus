# Microsoft Developer Studio Generated NMAKE File, Based on m_utils.dsp
!IF "$(CFG)" == ""
CFG=m_utils - Win32 DebugDll
!MESSAGE No configuration specified. Defaulting to m_utils - Win32 DebugDll.
!ENDIF 

!IF "$(CFG)" != "m_utils - Win32 Release" && "$(CFG)" != "m_utils - Win32 Debug" && "$(CFG)" != "m_utils - Win32 DebugDll" && "$(CFG)" != "m_utils - Win32 ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "m_utils.mak" CFG="m_utils - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "m_utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "m_utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "m_utils - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "m_utils - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "m_utils - Win32 Release"

OUTDIR=.\../../Out/ReleaseS
INTDIR=.\../../Temp/ReleaseS/CommonFiles/m_utils
# Begin Custom Macros
OutDir=.\../../Out/ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\m_utils.lib" "..\Release\m_utils.lib"


CLEAN :
	-@erase "$(INTDIR)\m_memcmp.obj"
	-@erase "$(INTDIR)\m_memcpy.obj"
	-@erase "$(INTDIR)\m_memmov.obj"
	-@erase "$(INTDIR)\m_memset.obj"
	-@erase "$(INTDIR)\m_strcat.obj"
	-@erase "$(INTDIR)\m_strcmp.obj"
	-@erase "$(INTDIR)\m_strcpy.obj"
	-@erase "$(INTDIR)\m_strlen.obj"
	-@erase "$(INTDIR)\m_strncp.obj"
	-@erase "$(INTDIR)\m_wcscat.obj"
	-@erase "$(INTDIR)\m_wcscmp.obj"
	-@erase "$(INTDIR)\m_wcscpy.obj"
	-@erase "$(INTDIR)\m_wcslen.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xtoa.obj"
	-@erase "$(OUTDIR)\m_utils.lib"
	-@erase "..\Release\m_utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /Ob0 /I "../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\m_utils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\m_utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\m_memcmp.obj" \
	"$(INTDIR)\m_memcpy.obj" \
	"$(INTDIR)\m_memmov.obj" \
	"$(INTDIR)\m_memset.obj" \
	"$(INTDIR)\m_strcat.obj" \
	"$(INTDIR)\m_strcmp.obj" \
	"$(INTDIR)\m_strcpy.obj" \
	"$(INTDIR)\m_strlen.obj" \
	"$(INTDIR)\m_wcscat.obj" \
	"$(INTDIR)\m_wcscmp.obj" \
	"$(INTDIR)\m_wcscpy.obj" \
	"$(INTDIR)\m_wcslen.obj" \
	"$(INTDIR)\xtoa.obj" \
	"$(INTDIR)\chkesp.obj" \
	"$(INTDIR)\lldiv.obj" \
	"$(INTDIR)\llmul.obj" \
	"$(INTDIR)\llrem.obj" \
	"$(INTDIR)\ulldiv.obj" \
	"$(INTDIR)\ullrem.obj" \
	"$(INTDIR)\m_strncp.obj"

"$(OUTDIR)\m_utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseS\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\Release\m_utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Release
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

OUTDIR=.\../../Out/DebugS
INTDIR=.\../../Temp/DebugS/CommonFiles/m_utils
# Begin Custom Macros
OutDir=.\../../Out/DebugS
# End Custom Macros

ALL : "$(OUTDIR)\m_utils.lib" "$(OUTDIR)\m_utils.bsc" "..\Debug\m_utils.lib"


CLEAN :
	-@erase "$(INTDIR)\dbg_rpt.obj"
	-@erase "$(INTDIR)\dbg_rpt.sbr"
	-@erase "$(INTDIR)\m_memcmp.obj"
	-@erase "$(INTDIR)\m_memcmp.sbr"
	-@erase "$(INTDIR)\m_memcpy.obj"
	-@erase "$(INTDIR)\m_memcpy.sbr"
	-@erase "$(INTDIR)\m_memmov.obj"
	-@erase "$(INTDIR)\m_memmov.sbr"
	-@erase "$(INTDIR)\m_memset.obj"
	-@erase "$(INTDIR)\m_memset.sbr"
	-@erase "$(INTDIR)\m_strcat.obj"
	-@erase "$(INTDIR)\m_strcat.sbr"
	-@erase "$(INTDIR)\m_strcmp.obj"
	-@erase "$(INTDIR)\m_strcmp.sbr"
	-@erase "$(INTDIR)\m_strcpy.obj"
	-@erase "$(INTDIR)\m_strcpy.sbr"
	-@erase "$(INTDIR)\m_strlen.obj"
	-@erase "$(INTDIR)\m_strlen.sbr"
	-@erase "$(INTDIR)\m_strncp.obj"
	-@erase "$(INTDIR)\m_strncp.sbr"
	-@erase "$(INTDIR)\m_wcscat.obj"
	-@erase "$(INTDIR)\m_wcscat.sbr"
	-@erase "$(INTDIR)\m_wcscmp.obj"
	-@erase "$(INTDIR)\m_wcscmp.sbr"
	-@erase "$(INTDIR)\m_wcscpy.obj"
	-@erase "$(INTDIR)\m_wcscpy.sbr"
	-@erase "$(INTDIR)\m_wcslen.obj"
	-@erase "$(INTDIR)\m_wcslen.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xtoa.obj"
	-@erase "$(INTDIR)\xtoa.sbr"
	-@erase "$(OUTDIR)\m_utils.bsc"
	-@erase "$(OUTDIR)\m_utils.lib"
	-@erase "..\Debug\m_utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\m_utils.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dbg_rpt.sbr" \
	"$(INTDIR)\m_memcmp.sbr" \
	"$(INTDIR)\m_memcpy.sbr" \
	"$(INTDIR)\m_memmov.sbr" \
	"$(INTDIR)\m_memset.sbr" \
	"$(INTDIR)\m_strcat.sbr" \
	"$(INTDIR)\m_strcmp.sbr" \
	"$(INTDIR)\m_strcpy.sbr" \
	"$(INTDIR)\m_strlen.sbr" \
	"$(INTDIR)\m_wcscat.sbr" \
	"$(INTDIR)\m_wcscmp.sbr" \
	"$(INTDIR)\m_wcscpy.sbr" \
	"$(INTDIR)\m_wcslen.sbr" \
	"$(INTDIR)\xtoa.sbr" \
	"$(INTDIR)\m_strncp.sbr"

"$(OUTDIR)\m_utils.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\m_utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbg_rpt.obj" \
	"$(INTDIR)\m_memcmp.obj" \
	"$(INTDIR)\m_memcpy.obj" \
	"$(INTDIR)\m_memmov.obj" \
	"$(INTDIR)\m_memset.obj" \
	"$(INTDIR)\m_strcat.obj" \
	"$(INTDIR)\m_strcmp.obj" \
	"$(INTDIR)\m_strcpy.obj" \
	"$(INTDIR)\m_strlen.obj" \
	"$(INTDIR)\m_wcscat.obj" \
	"$(INTDIR)\m_wcscmp.obj" \
	"$(INTDIR)\m_wcscpy.obj" \
	"$(INTDIR)\m_wcslen.obj" \
	"$(INTDIR)\xtoa.obj" \
	"$(INTDIR)\chkesp.obj" \
	"$(INTDIR)\lldiv.obj" \
	"$(INTDIR)\llmul.obj" \
	"$(INTDIR)\llrem.obj" \
	"$(INTDIR)\ulldiv.obj" \
	"$(INTDIR)\ullrem.obj" \
	"$(INTDIR)\m_strncp.obj"

"$(OUTDIR)\m_utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugS\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\Debug\m_utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

OUTDIR=.\../../Out/Debug
INTDIR=.\../../Temp/Debug/CommonFiles/m_utils
# Begin Custom Macros
OutDir=.\../../Out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\m_utils.lib" ".\DebugDll\m_utils.bsc" "..\DebugDll\m_utils.lib"


CLEAN :
	-@erase "$(INTDIR)\dbg_rpt.obj"
	-@erase "$(INTDIR)\dbg_rpt.sbr"
	-@erase "$(INTDIR)\m_memcmp.obj"
	-@erase "$(INTDIR)\m_memcmp.sbr"
	-@erase "$(INTDIR)\m_memcpy.obj"
	-@erase "$(INTDIR)\m_memcpy.sbr"
	-@erase "$(INTDIR)\m_memmov.obj"
	-@erase "$(INTDIR)\m_memmov.sbr"
	-@erase "$(INTDIR)\m_memset.obj"
	-@erase "$(INTDIR)\m_memset.sbr"
	-@erase "$(INTDIR)\m_strcat.obj"
	-@erase "$(INTDIR)\m_strcat.sbr"
	-@erase "$(INTDIR)\m_strcmp.obj"
	-@erase "$(INTDIR)\m_strcmp.sbr"
	-@erase "$(INTDIR)\m_strcpy.obj"
	-@erase "$(INTDIR)\m_strcpy.sbr"
	-@erase "$(INTDIR)\m_strlen.obj"
	-@erase "$(INTDIR)\m_strlen.sbr"
	-@erase "$(INTDIR)\m_strncp.obj"
	-@erase "$(INTDIR)\m_strncp.sbr"
	-@erase "$(INTDIR)\m_wcscat.obj"
	-@erase "$(INTDIR)\m_wcscat.sbr"
	-@erase "$(INTDIR)\m_wcscmp.obj"
	-@erase "$(INTDIR)\m_wcscmp.sbr"
	-@erase "$(INTDIR)\m_wcscpy.obj"
	-@erase "$(INTDIR)\m_wcscpy.sbr"
	-@erase "$(INTDIR)\m_wcslen.obj"
	-@erase "$(INTDIR)\m_wcslen.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xtoa.obj"
	-@erase "$(INTDIR)\xtoa.sbr"
	-@erase "$(OUTDIR)\m_utils.lib"
	-@erase ".\DebugDll\m_utils.bsc"
	-@erase "..\DebugDll\m_utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"DebugDll/m_utils.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dbg_rpt.sbr" \
	"$(INTDIR)\m_memcmp.sbr" \
	"$(INTDIR)\m_memcpy.sbr" \
	"$(INTDIR)\m_memmov.sbr" \
	"$(INTDIR)\m_memset.sbr" \
	"$(INTDIR)\m_strcat.sbr" \
	"$(INTDIR)\m_strcmp.sbr" \
	"$(INTDIR)\m_strcpy.sbr" \
	"$(INTDIR)\m_strlen.sbr" \
	"$(INTDIR)\m_wcscat.sbr" \
	"$(INTDIR)\m_wcscmp.sbr" \
	"$(INTDIR)\m_wcscpy.sbr" \
	"$(INTDIR)\m_wcslen.sbr" \
	"$(INTDIR)\xtoa.sbr" \
	"$(INTDIR)\m_strncp.sbr"

".\DebugDll\m_utils.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\m_utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbg_rpt.obj" \
	"$(INTDIR)\m_memcmp.obj" \
	"$(INTDIR)\m_memcpy.obj" \
	"$(INTDIR)\m_memmov.obj" \
	"$(INTDIR)\m_memset.obj" \
	"$(INTDIR)\m_strcat.obj" \
	"$(INTDIR)\m_strcmp.obj" \
	"$(INTDIR)\m_strcpy.obj" \
	"$(INTDIR)\m_strlen.obj" \
	"$(INTDIR)\m_wcscat.obj" \
	"$(INTDIR)\m_wcscmp.obj" \
	"$(INTDIR)\m_wcscpy.obj" \
	"$(INTDIR)\m_wcslen.obj" \
	"$(INTDIR)\xtoa.obj" \
	"$(INTDIR)\chkesp.obj" \
	"$(INTDIR)\lldiv.obj" \
	"$(INTDIR)\llmul.obj" \
	"$(INTDIR)\llrem.obj" \
	"$(INTDIR)\ulldiv.obj" \
	"$(INTDIR)\ullrem.obj" \
	"$(INTDIR)\m_strncp.obj"

"$(OUTDIR)\m_utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Debug\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\DebugDll\m_utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

OUTDIR=.\../../Out/Release
INTDIR=.\../../Temp/Release/CommonFiles/m_utils
# Begin Custom Macros
OutDir=.\../../Out/Release
# End Custom Macros

ALL : "$(OUTDIR)\m_utils.lib" "..\ReleaseDll\m_utils.lib"


CLEAN :
	-@erase "$(INTDIR)\m_memcmp.obj"
	-@erase "$(INTDIR)\m_memcpy.obj"
	-@erase "$(INTDIR)\m_memmov.obj"
	-@erase "$(INTDIR)\m_memset.obj"
	-@erase "$(INTDIR)\m_strcat.obj"
	-@erase "$(INTDIR)\m_strcmp.obj"
	-@erase "$(INTDIR)\m_strcpy.obj"
	-@erase "$(INTDIR)\m_strlen.obj"
	-@erase "$(INTDIR)\m_strncp.obj"
	-@erase "$(INTDIR)\m_wcscat.obj"
	-@erase "$(INTDIR)\m_wcscmp.obj"
	-@erase "$(INTDIR)\m_wcscpy.obj"
	-@erase "$(INTDIR)\m_wcslen.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xtoa.obj"
	-@erase "$(OUTDIR)\m_utils.lib"
	-@erase "..\ReleaseDll\m_utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob0 /I "../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "M_UTILS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"ReleaseDll/m_utils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\m_utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\m_memcmp.obj" \
	"$(INTDIR)\m_memcpy.obj" \
	"$(INTDIR)\m_memmov.obj" \
	"$(INTDIR)\m_memset.obj" \
	"$(INTDIR)\m_strcat.obj" \
	"$(INTDIR)\m_strcmp.obj" \
	"$(INTDIR)\m_strcpy.obj" \
	"$(INTDIR)\m_strlen.obj" \
	"$(INTDIR)\m_wcscat.obj" \
	"$(INTDIR)\m_wcscmp.obj" \
	"$(INTDIR)\m_wcscpy.obj" \
	"$(INTDIR)\m_wcslen.obj" \
	"$(INTDIR)\xtoa.obj" \
	"$(INTDIR)\chkesp.obj" \
	"$(INTDIR)\lldiv.obj" \
	"$(INTDIR)\llmul.obj" \
	"$(INTDIR)\llrem.obj" \
	"$(INTDIR)\ulldiv.obj" \
	"$(INTDIR)\ullrem.obj" \
	"$(INTDIR)\m_strncp.obj"

"$(OUTDIR)\m_utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Release\m_utils.lib
InputName=m_utils
SOURCE="$(InputPath)"

"..\ReleaseDll\m_utils.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)"  ..\ReleaseDll
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
!IF EXISTS("m_utils.dep")
!INCLUDE "m_utils.dep"
!ELSE 
!MESSAGE Warning: cannot find "m_utils.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "m_utils - Win32 Release" || "$(CFG)" == "m_utils - Win32 Debug" || "$(CFG)" == "m_utils - Win32 DebugDll" || "$(CFG)" == "m_utils - Win32 ReleaseDll"
SOURCE=.\chkesp.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(INTDIR)\chkesp.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /Fo$(IntDir)\$(InputName).obj $(InputPath) 
	ml /c /coff /Fo$(IntDir)\$(InputName).obj $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(INTDIR)\chkesp.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(INTDIR)\chkesp.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\chkesp.asm
InputName=chkesp

"$(INTDIR)\chkesp.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\dbg_rpt.c

!IF  "$(CFG)" == "m_utils - Win32 Release"

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\dbg_rpt.obj"	"$(INTDIR)\dbg_rpt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\dbg_rpt.obj"	"$(INTDIR)\dbg_rpt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

!ENDIF 

SOURCE=.\lldiv.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(INTDIR)\lldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(INTDIR)\lldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(INTDIR)\lldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\lldiv.asm
InputName=lldiv

"$(INTDIR)\lldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\llmul.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(INTDIR)\llmul.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(INTDIR)\llmul.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(INTDIR)\llmul.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\llmul.asm
InputName=llmul

"$(INTDIR)\llmul.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\llrem.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(INTDIR)\llrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(INTDIR)\llrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(INTDIR)\llrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\llrem.asm
InputName=llrem

"$(INTDIR)\llrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\m_memcmp.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_memcmp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_memcmp.obj"	"$(INTDIR)\m_memcmp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_memcmp.obj"	"$(INTDIR)\m_memcmp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_memcmp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_memcpy.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_memcpy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_memcpy.obj"	"$(INTDIR)\m_memcpy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_memcpy.obj"	"$(INTDIR)\m_memcpy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_memcpy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_memmov.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_memmov.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_memmov.obj"	"$(INTDIR)\m_memmov.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_memmov.obj"	"$(INTDIR)\m_memmov.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_memmov.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_memset.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_memset.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_memset.obj"	"$(INTDIR)\m_memset.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_memset.obj"	"$(INTDIR)\m_memset.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_memset.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_strcat.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_strcat.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_strcat.obj"	"$(INTDIR)\m_strcat.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_strcat.obj"	"$(INTDIR)\m_strcat.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_strcat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_strcmp.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_strcmp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_strcmp.obj"	"$(INTDIR)\m_strcmp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_strcmp.obj"	"$(INTDIR)\m_strcmp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_strcmp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_strcpy.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_strcpy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_strcpy.obj"	"$(INTDIR)\m_strcpy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_strcpy.obj"	"$(INTDIR)\m_strcpy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_strcpy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_strlen.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_strlen.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_strlen.obj"	"$(INTDIR)\m_strlen.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_strlen.obj"	"$(INTDIR)\m_strlen.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_strlen.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_strncp.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_strncp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_strncp.obj"	"$(INTDIR)\m_strncp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_strncp.obj"	"$(INTDIR)\m_strncp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_strncp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_wcscat.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_wcscat.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_wcscat.obj"	"$(INTDIR)\m_wcscat.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_wcscat.obj"	"$(INTDIR)\m_wcscat.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_wcscat.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_wcscmp.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_wcscmp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_wcscmp.obj"	"$(INTDIR)\m_wcscmp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_wcscmp.obj"	"$(INTDIR)\m_wcscmp.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_wcscmp.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_wcscpy.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_wcscpy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_wcscpy.obj"	"$(INTDIR)\m_wcscpy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_wcscpy.obj"	"$(INTDIR)\m_wcscpy.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_wcscpy.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\m_wcslen.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\m_wcslen.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\m_wcslen.obj"	"$(INTDIR)\m_wcslen.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\m_wcslen.obj"	"$(INTDIR)\m_wcslen.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\m_wcslen.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ulldiv.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(INTDIR)\ulldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(INTDIR)\ulldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	/Zi /Zd echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(INTDIR)\ulldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\ulldiv.asm
InputName=ulldiv

"$(INTDIR)\ulldiv.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\ullrem.asm

!IF  "$(CFG)" == "m_utils - Win32 Release"

IntDir=.\../../Temp/ReleaseS/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(INTDIR)\ullrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"

IntDir=.\../../Temp/DebugS/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(INTDIR)\ullrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Zi /Zd /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"

IntDir=.\../../Temp/Debug/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(INTDIR)\ullrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	/Zi /Zd echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"

IntDir=.\../../Temp/Release/CommonFiles/m_utils
InputPath=.\ullrem.asm
InputName=ullrem

"$(INTDIR)\ullrem.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /DI386 /Fo$(IntDir)\$(InputName).obj /FR$(IntDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\xtoa.c

!IF  "$(CFG)" == "m_utils - Win32 Release"


"$(INTDIR)\xtoa.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 Debug"


"$(INTDIR)\xtoa.obj"	"$(INTDIR)\xtoa.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 DebugDll"


"$(INTDIR)\xtoa.obj"	"$(INTDIR)\xtoa.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "m_utils - Win32 ReleaseDll"


"$(INTDIR)\xtoa.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


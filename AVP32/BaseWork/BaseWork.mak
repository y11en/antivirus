# Microsoft Developer Studio Generated NMAKE File, Based on BaseWork.dsp
!IF "$(CFG)" == ""
CFG=BaseWork - Win32 Debug
!MESSAGE No configuration specified. Defaulting to BaseWork - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "BaseWork - Win32 Release" && "$(CFG)" != "BaseWork - Win32 Debug" && "$(CFG)" != "BaseWork - Win32 Release Static"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BaseWork.mak" CFG="BaseWork - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BaseWork - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BaseWork - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "BaseWork - Win32 Release Static" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "BaseWork - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release\AVP32\BaseWork
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\BaseWork.lib"


CLEAN :
	-@erase "$(INTDIR)\_carray.obj"
	-@erase "$(INTDIR)\_printf.obj"
	-@erase "$(INTDIR)\BaseAPI.obj"
	-@erase "$(INTDIR)\Basework.obj"
	-@erase "$(INTDIR)\BaseWork.pch"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\Clink.obj"
	-@erase "$(INTDIR)\DupInstance.obj"
	-@erase "$(INTDIR)\DupMemory.obj"
	-@erase "$(INTDIR)\FileIO.obj"
	-@erase "$(INTDIR)\ImgSect.obj"
	-@erase "$(INTDIR)\pagememorymanager.obj"
	-@erase "$(INTDIR)\SpcAlloc.obj"
	-@erase "$(INTDIR)\Sq_u.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\suballoc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\BaseWork.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fp"$(INTDIR)\BaseWork.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BaseWork.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\BaseWork.lib" 
LIB32_OBJS= \
	"$(INTDIR)\_carray.obj" \
	"$(INTDIR)\_printf.obj" \
	"$(INTDIR)\BaseAPI.obj" \
	"$(INTDIR)\Basework.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\Clink.obj" \
	"$(INTDIR)\DupInstance.obj" \
	"$(INTDIR)\DupMemory.obj" \
	"$(INTDIR)\FileIO.obj" \
	"$(INTDIR)\ImgSect.obj" \
	"$(INTDIR)\pagememorymanager.obj" \
	"$(INTDIR)\SpcAlloc.obj" \
	"$(INTDIR)\Sq_u.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\suballoc.obj"

"$(OUTDIR)\BaseWork.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug\AVP32\BaseWork
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\BaseWork.lib"


CLEAN :
	-@erase "$(INTDIR)\_carray.obj"
	-@erase "$(INTDIR)\_printf.obj"
	-@erase "$(INTDIR)\BaseAPI.obj"
	-@erase "$(INTDIR)\Basework.obj"
	-@erase "$(INTDIR)\BaseWork.pch"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\Clink.obj"
	-@erase "$(INTDIR)\DupInstance.obj"
	-@erase "$(INTDIR)\DupMemory.obj"
	-@erase "$(INTDIR)\FileIO.obj"
	-@erase "$(INTDIR)\ImgSect.obj"
	-@erase "$(INTDIR)\pagememorymanager.obj"
	-@erase "$(INTDIR)\SpcAlloc.obj"
	-@erase "$(INTDIR)\Sq_u.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\suballoc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\BaseWork.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fp"$(INTDIR)\BaseWork.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BaseWork.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\BaseWork.lib" 
LIB32_OBJS= \
	"$(INTDIR)\_carray.obj" \
	"$(INTDIR)\_printf.obj" \
	"$(INTDIR)\BaseAPI.obj" \
	"$(INTDIR)\Basework.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\Clink.obj" \
	"$(INTDIR)\DupInstance.obj" \
	"$(INTDIR)\DupMemory.obj" \
	"$(INTDIR)\FileIO.obj" \
	"$(INTDIR)\ImgSect.obj" \
	"$(INTDIR)\pagememorymanager.obj" \
	"$(INTDIR)\SpcAlloc.obj" \
	"$(INTDIR)\Sq_u.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\suballoc.obj"

"$(OUTDIR)\BaseWork.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

OUTDIR=.\..\..\out\release_static
INTDIR=.\..\..\temp\release_static
# Begin Custom Macros
OutDir=.\..\..\out\release_static
# End Custom Macros

ALL : "$(OUTDIR)\BaseWork.lib"


CLEAN :
	-@erase "$(INTDIR)\_carray.obj"
	-@erase "$(INTDIR)\_printf.obj"
	-@erase "$(INTDIR)\BaseAPI.obj"
	-@erase "$(INTDIR)\Basework.obj"
	-@erase "$(INTDIR)\BaseWork.pch"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\Clink.obj"
	-@erase "$(INTDIR)\DupInstance.obj"
	-@erase "$(INTDIR)\DupMemory.obj"
	-@erase "$(INTDIR)\ImgSect.obj"
	-@erase "$(INTDIR)\pagememorymanager.obj"
	-@erase "$(INTDIR)\SpcAlloc.obj"
	-@erase "$(INTDIR)\Sq_u.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\suballoc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\BaseWork.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Fp"$(INTDIR)\BaseWork.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BaseWork.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\BaseWork.lib" 
LIB32_OBJS= \
	"$(INTDIR)\_carray.obj" \
	"$(INTDIR)\_printf.obj" \
	"$(INTDIR)\BaseAPI.obj" \
	"$(INTDIR)\Basework.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\Clink.obj" \
	"$(INTDIR)\DupInstance.obj" \
	"$(INTDIR)\DupMemory.obj" \
	"$(INTDIR)\ImgSect.obj" \
	"$(INTDIR)\pagememorymanager.obj" \
	"$(INTDIR)\SpcAlloc.obj" \
	"$(INTDIR)\Sq_u.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\suballoc.obj"

"$(OUTDIR)\BaseWork.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("BaseWork.dep")
!INCLUDE "BaseWork.dep"
!ELSE 
!MESSAGE Warning: cannot find "BaseWork.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "BaseWork - Win32 Release" || "$(CFG)" == "BaseWork - Win32 Debug" || "$(CFG)" == "BaseWork - Win32 Release Static"
SOURCE=..\..\CommonFiles\Stuff\_carray.cpp

"$(INTDIR)\_carray.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\_printf.cpp

"$(INTDIR)\_printf.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"


SOURCE=.\BaseAPI.cpp

"$(INTDIR)\BaseAPI.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"


SOURCE=.\Basework.cpp

"$(INTDIR)\Basework.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"


SOURCE=..\..\CommonFiles\Stuff\CalcSum.c

!IF  "$(CFG)" == "BaseWork - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CalcSum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\CalcSum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

CPP_SWITCHES=/nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CalcSum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Clink.cpp

"$(INTDIR)\Clink.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"


SOURCE=.\DupInstance.cpp

"$(INTDIR)\DupInstance.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"


SOURCE=.\DupMemory.cpp

"$(INTDIR)\DupMemory.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"


SOURCE=..\IOCache\FileIO.cpp

!IF  "$(CFG)" == "BaseWork - Win32 Release"


"$(INTDIR)\FileIO.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"


"$(INTDIR)\FileIO.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

!ENDIF 

SOURCE=.\ImgSect.c

!IF  "$(CFG)" == "BaseWork - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ImgSect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ImgSect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

CPP_SWITCHES=/nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ImgSect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\pagememorymanager\pagememorymanager.cpp

!IF  "$(CFG)" == "BaseWork - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pagememorymanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pagememorymanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

CPP_SWITCHES=/nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pagememorymanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\SpcAlloc.cpp

!IF  "$(CFG)" == "BaseWork - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\SpcAlloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\SpcAlloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

CPP_SWITCHES=/nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\SpcAlloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\CommonFiles\Stuff\Sq_u.cpp

"$(INTDIR)\Sq_u.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "BaseWork - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fp"$(INTDIR)\BaseWork.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\BaseWork.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /D "GMTMODIFICATIONTIME" /Fp"$(INTDIR)\BaseWork.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\BaseWork.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BaseWork - Win32 Release Static"

CPP_SWITCHES=/nologo /G5 /MT /W3 /Zi /O2 /I "." /I "..\\" /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MULTITHREAD" /Fp"$(INTDIR)\BaseWork.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\BaseWork.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\suballoc.cpp

"$(INTDIR)\suballoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BaseWork.pch"



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on png.dsp
!IF "$(CFG)" == ""
CFG=png - Win32 Release
!MESSAGE No configuration specified. Defaulting to png - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "png - Win32 Release" && "$(CFG)" != "png - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "png.mak" CFG="png - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "png - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "png - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "png - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\Release\png.lib"


CLEAN :
	-@erase "$(INTDIR)\Png.obj"
	-@erase "$(INTDIR)\Pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\Pngmem.obj"
	-@erase "$(INTDIR)\Pngpread.obj"
	-@erase "$(INTDIR)\Pngread.obj"
	-@erase "$(INTDIR)\Pngrio.obj"
	-@erase "$(INTDIR)\Pngrtran.obj"
	-@erase "$(INTDIR)\Pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\Pngtest.obj"
	-@erase "$(INTDIR)\Pngtrans.obj"
	-@erase "$(INTDIR)\Pngwio.obj"
	-@erase "$(INTDIR)\Pngwrite.obj"
	-@erase "$(INTDIR)\Pngwtran.obj"
	-@erase "$(INTDIR)\Pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\Release\png.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\png.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\Release\png.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Png.obj" \
	"$(INTDIR)\Pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\Pngmem.obj" \
	"$(INTDIR)\Pngpread.obj" \
	"$(INTDIR)\Pngread.obj" \
	"$(INTDIR)\Pngrio.obj" \
	"$(INTDIR)\Pngrtran.obj" \
	"$(INTDIR)\Pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\Pngtest.obj" \
	"$(INTDIR)\Pngtrans.obj" \
	"$(INTDIR)\Pngwio.obj" \
	"$(INTDIR)\Pngwrite.obj" \
	"$(INTDIR)\Pngwtran.obj" \
	"$(INTDIR)\Pngwutil.obj"

"..\..\Release\png.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "png - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\Debug\png.lib"


CLEAN :
	-@erase "$(INTDIR)\Png.obj"
	-@erase "$(INTDIR)\Pngerror.obj"
	-@erase "$(INTDIR)\pngget.obj"
	-@erase "$(INTDIR)\Pngmem.obj"
	-@erase "$(INTDIR)\Pngpread.obj"
	-@erase "$(INTDIR)\Pngread.obj"
	-@erase "$(INTDIR)\Pngrio.obj"
	-@erase "$(INTDIR)\Pngrtran.obj"
	-@erase "$(INTDIR)\Pngrutil.obj"
	-@erase "$(INTDIR)\pngset.obj"
	-@erase "$(INTDIR)\Pngtest.obj"
	-@erase "$(INTDIR)\Pngtrans.obj"
	-@erase "$(INTDIR)\Pngwio.obj"
	-@erase "$(INTDIR)\Pngwrite.obj"
	-@erase "$(INTDIR)\Pngwtran.obj"
	-@erase "$(INTDIR)\Pngwutil.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\Debug\png.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /I "..\zlib" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\png.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\Debug\png.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Png.obj" \
	"$(INTDIR)\Pngerror.obj" \
	"$(INTDIR)\pngget.obj" \
	"$(INTDIR)\Pngmem.obj" \
	"$(INTDIR)\Pngpread.obj" \
	"$(INTDIR)\Pngread.obj" \
	"$(INTDIR)\Pngrio.obj" \
	"$(INTDIR)\Pngrtran.obj" \
	"$(INTDIR)\Pngrutil.obj" \
	"$(INTDIR)\pngset.obj" \
	"$(INTDIR)\Pngtest.obj" \
	"$(INTDIR)\Pngtrans.obj" \
	"$(INTDIR)\Pngwio.obj" \
	"$(INTDIR)\Pngwrite.obj" \
	"$(INTDIR)\Pngwtran.obj" \
	"$(INTDIR)\Pngwutil.obj"

"..\..\Debug\png.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("png.dep")
!INCLUDE "png.dep"
!ELSE 
!MESSAGE Warning: cannot find "png.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "png - Win32 Release" || "$(CFG)" == "png - Win32 Debug"
SOURCE=.\Png.c

"$(INTDIR)\Png.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngerror.c

"$(INTDIR)\Pngerror.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pngget.c

"$(INTDIR)\pngget.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngmem.c

"$(INTDIR)\Pngmem.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngpread.c

"$(INTDIR)\Pngpread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngread.c

"$(INTDIR)\Pngread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngrio.c

"$(INTDIR)\Pngrio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngrtran.c

"$(INTDIR)\Pngrtran.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngrutil.c

"$(INTDIR)\Pngrutil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pngset.c

"$(INTDIR)\pngset.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngtest.c

"$(INTDIR)\Pngtest.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngtrans.c

"$(INTDIR)\Pngtrans.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngwio.c

"$(INTDIR)\Pngwio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngwrite.c

"$(INTDIR)\Pngwrite.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngwtran.c

"$(INTDIR)\Pngwtran.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Pngwutil.c

"$(INTDIR)\Pngwutil.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


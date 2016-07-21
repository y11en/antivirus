# Microsoft Developer Studio Generated NMAKE File, Based on zlib.dsp
!IF "$(CFG)" == ""
CFG=zlib - Win32 Release
!MESSAGE No configuration specified. Defaulting to zlib - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "zlib - Win32 Release" && "$(CFG)" != "zlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "zlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\Release\zlib.lib"


CLEAN :
	-@erase "$(INTDIR)\Adler32.obj"
	-@erase "$(INTDIR)\Compress.obj"
	-@erase "$(INTDIR)\Crc32.obj"
	-@erase "$(INTDIR)\Deflate.obj"
	-@erase "$(INTDIR)\Gzio.obj"
	-@erase "$(INTDIR)\Infblock.obj"
	-@erase "$(INTDIR)\Infcodes.obj"
	-@erase "$(INTDIR)\Inffast.obj"
	-@erase "$(INTDIR)\Inflate.obj"
	-@erase "$(INTDIR)\Inftrees.obj"
	-@erase "$(INTDIR)\Infutil.obj"
	-@erase "$(INTDIR)\Trees.obj"
	-@erase "$(INTDIR)\Uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Zutil.obj"
	-@erase "..\..\Release\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\Release\zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Adler32.obj" \
	"$(INTDIR)\Compress.obj" \
	"$(INTDIR)\Crc32.obj" \
	"$(INTDIR)\Deflate.obj" \
	"$(INTDIR)\Gzio.obj" \
	"$(INTDIR)\Infblock.obj" \
	"$(INTDIR)\Infcodes.obj" \
	"$(INTDIR)\Inffast.obj" \
	"$(INTDIR)\Inflate.obj" \
	"$(INTDIR)\Inftrees.obj" \
	"$(INTDIR)\Infutil.obj" \
	"$(INTDIR)\Trees.obj" \
	"$(INTDIR)\Uncompr.obj" \
	"$(INTDIR)\Zutil.obj"

"..\..\Release\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\Debug\zlib.lib"


CLEAN :
	-@erase "$(INTDIR)\Adler32.obj"
	-@erase "$(INTDIR)\Compress.obj"
	-@erase "$(INTDIR)\Crc32.obj"
	-@erase "$(INTDIR)\Deflate.obj"
	-@erase "$(INTDIR)\Gzio.obj"
	-@erase "$(INTDIR)\Infblock.obj"
	-@erase "$(INTDIR)\Infcodes.obj"
	-@erase "$(INTDIR)\Inffast.obj"
	-@erase "$(INTDIR)\Inflate.obj"
	-@erase "$(INTDIR)\Inftrees.obj"
	-@erase "$(INTDIR)\Infutil.obj"
	-@erase "$(INTDIR)\Trees.obj"
	-@erase "$(INTDIR)\Uncompr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\Zutil.obj"
	-@erase "..\..\Debug\zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\Debug\zlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Adler32.obj" \
	"$(INTDIR)\Compress.obj" \
	"$(INTDIR)\Crc32.obj" \
	"$(INTDIR)\Deflate.obj" \
	"$(INTDIR)\Gzio.obj" \
	"$(INTDIR)\Infblock.obj" \
	"$(INTDIR)\Infcodes.obj" \
	"$(INTDIR)\Inffast.obj" \
	"$(INTDIR)\Inflate.obj" \
	"$(INTDIR)\Inftrees.obj" \
	"$(INTDIR)\Infutil.obj" \
	"$(INTDIR)\Trees.obj" \
	"$(INTDIR)\Uncompr.obj" \
	"$(INTDIR)\Zutil.obj"

"..\..\Debug\zlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("zlib.dep")
!INCLUDE "zlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "zlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "zlib - Win32 Release" || "$(CFG)" == "zlib - Win32 Debug"
SOURCE=.\Adler32.c

"$(INTDIR)\Adler32.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Compress.c

"$(INTDIR)\Compress.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Crc32.c

"$(INTDIR)\Crc32.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Deflate.c

"$(INTDIR)\Deflate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Gzio.c

"$(INTDIR)\Gzio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Infblock.c

"$(INTDIR)\Infblock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Infcodes.c

"$(INTDIR)\Infcodes.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Inffast.c

"$(INTDIR)\Inffast.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Inflate.c

"$(INTDIR)\Inflate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Inftrees.c

"$(INTDIR)\Inftrees.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Infutil.c

"$(INTDIR)\Infutil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Trees.c

"$(INTDIR)\Trees.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Uncompr.c

"$(INTDIR)\Uncompr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Zutil.c

"$(INTDIR)\Zutil.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on rar.dsp
!IF "$(CFG)" == ""
CFG=rar - Win32 Debug
!MESSAGE No configuration specified. Defaulting to rar - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "rar - Win32 Release" && "$(CFG)" != "rar - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rar.mak" CFG="rar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rar - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rar - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "rar - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/MiniArchiver/Rar
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\rar.ppl"


CLEAN :
	-@erase "$(INTDIR)\cblock.obj"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\model.obj"
	-@erase "$(INTDIR)\mtree.obj"
	-@erase "$(INTDIR)\pack20.obj"
	-@erase "$(INTDIR)\rar.obj"
	-@erase "$(INTDIR)\rar.res"
	-@erase "$(INTDIR)\rarvm.obj"
	-@erase "$(INTDIR)\rijndael.obj"
	-@erase "$(INTDIR)\sha1.obj"
	-@erase "$(INTDIR)\stdflt.obj"
	-@erase "$(INTDIR)\suballoc.obj"
	-@erase "$(INTDIR)\unpack15.obj"
	-@erase "$(INTDIR)\unpack20.obj"
	-@erase "$(INTDIR)\unpack29.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rar.exp"
	-@erase "$(OUTDIR)\rar.pdb"
	-@erase "$(OUTDIR)\rar.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rar.res" /i "../../../CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib m_utils.lib /nologo /base:"0x66b00000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\rar.pdb" /debug /machine:I386 /out:"$(OUTDIR)\rar.ppl" /implib:"$(OUTDIR)\rar.lib" /pdbtype:sept /libpath:"..\..\..\out\Release" /libpath:"..\..\..\CommonFiles\ReleaseDll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\rar.obj" \
	"$(INTDIR)\cblock.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\model.obj" \
	"$(INTDIR)\mtree.obj" \
	"$(INTDIR)\pack20.obj" \
	"$(INTDIR)\rarvm.obj" \
	"$(INTDIR)\rijndael.obj" \
	"$(INTDIR)\sha1.obj" \
	"$(INTDIR)\stdflt.obj" \
	"$(INTDIR)\suballoc.obj" \
	"$(INTDIR)\unpack15.obj" \
	"$(INTDIR)\unpack20.obj" \
	"$(INTDIR)\unpack29.obj" \
	"$(INTDIR)\rar.res"

"$(OUTDIR)\rar.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\rar.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\rar.ppl"
   call prconvert "\out\Release\rar.ppl"
	tsigner "\out\Release\rar.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "rar - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/MiniArchiver/Rar
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\rar.ppl" "$(OUTDIR)\rar.bsc"


CLEAN :
	-@erase "$(INTDIR)\cblock.obj"
	-@erase "$(INTDIR)\cblock.sbr"
	-@erase "$(INTDIR)\common.obj"
	-@erase "$(INTDIR)\common.sbr"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\crc.sbr"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\crypt.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\mem.obj"
	-@erase "$(INTDIR)\mem.sbr"
	-@erase "$(INTDIR)\model.obj"
	-@erase "$(INTDIR)\model.sbr"
	-@erase "$(INTDIR)\mtree.obj"
	-@erase "$(INTDIR)\mtree.sbr"
	-@erase "$(INTDIR)\pack20.obj"
	-@erase "$(INTDIR)\pack20.sbr"
	-@erase "$(INTDIR)\rar.obj"
	-@erase "$(INTDIR)\rar.res"
	-@erase "$(INTDIR)\rar.sbr"
	-@erase "$(INTDIR)\rarvm.obj"
	-@erase "$(INTDIR)\rarvm.sbr"
	-@erase "$(INTDIR)\rijndael.obj"
	-@erase "$(INTDIR)\rijndael.sbr"
	-@erase "$(INTDIR)\sha1.obj"
	-@erase "$(INTDIR)\sha1.sbr"
	-@erase "$(INTDIR)\stdflt.obj"
	-@erase "$(INTDIR)\stdflt.sbr"
	-@erase "$(INTDIR)\suballoc.obj"
	-@erase "$(INTDIR)\suballoc.sbr"
	-@erase "$(INTDIR)\unpack15.obj"
	-@erase "$(INTDIR)\unpack15.sbr"
	-@erase "$(INTDIR)\unpack20.obj"
	-@erase "$(INTDIR)\unpack20.sbr"
	-@erase "$(INTDIR)\unpack29.obj"
	-@erase "$(INTDIR)\unpack29.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\rar.bsc"
	-@erase "$(OUTDIR)\rar.exp"
	-@erase "$(OUTDIR)\rar.ilk"
	-@erase "$(OUTDIR)\rar.pdb"
	-@erase "$(OUTDIR)\rar.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\rar.res" /i "../../../CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\rar.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\rar.sbr" \
	"$(INTDIR)\cblock.sbr" \
	"$(INTDIR)\common.sbr" \
	"$(INTDIR)\crc.sbr" \
	"$(INTDIR)\crypt.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\mem.sbr" \
	"$(INTDIR)\model.sbr" \
	"$(INTDIR)\mtree.sbr" \
	"$(INTDIR)\pack20.sbr" \
	"$(INTDIR)\rarvm.sbr" \
	"$(INTDIR)\rijndael.sbr" \
	"$(INTDIR)\sha1.sbr" \
	"$(INTDIR)\stdflt.sbr" \
	"$(INTDIR)\suballoc.sbr" \
	"$(INTDIR)\unpack15.sbr" \
	"$(INTDIR)\unpack20.sbr" \
	"$(INTDIR)\unpack29.sbr"

"$(OUTDIR)\rar.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\rar.pdb" /debug /machine:I386 /out:"$(OUTDIR)\rar.ppl" /implib:"$(OUTDIR)\rar.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\rar.obj" \
	"$(INTDIR)\cblock.obj" \
	"$(INTDIR)\common.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\mem.obj" \
	"$(INTDIR)\model.obj" \
	"$(INTDIR)\mtree.obj" \
	"$(INTDIR)\pack20.obj" \
	"$(INTDIR)\rarvm.obj" \
	"$(INTDIR)\rijndael.obj" \
	"$(INTDIR)\sha1.obj" \
	"$(INTDIR)\stdflt.obj" \
	"$(INTDIR)\suballoc.obj" \
	"$(INTDIR)\unpack15.obj" \
	"$(INTDIR)\unpack20.obj" \
	"$(INTDIR)\unpack29.obj" \
	"$(INTDIR)\rar.res"

"$(OUTDIR)\rar.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("rar.dep")
!INCLUDE "rar.dep"
!ELSE 
!MESSAGE Warning: cannot find "rar.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "rar - Win32 Release" || "$(CFG)" == "rar - Win32 Debug"
SOURCE=.\main.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rar.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\rar.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\rar.obj"	"$(INTDIR)\rar.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rar.rc

"$(INTDIR)\rar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\base\cblock.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\cblock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\cblock.obj"	"$(INTDIR)\cblock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\common.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\common.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\common.obj"	"$(INTDIR)\common.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\crc.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\crc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\crc.obj"	"$(INTDIR)\crc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\crypt.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\crypt.obj"	"$(INTDIR)\crypt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\input.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\input.obj"	"$(INTDIR)\input.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\mem.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\mem.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\mem.obj"	"$(INTDIR)\mem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\model.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\model.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\model.obj"	"$(INTDIR)\model.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\mtree.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\mtree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\mtree.obj"	"$(INTDIR)\mtree.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\pack20.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\pack20.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\pack20.obj"	"$(INTDIR)\pack20.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\rarvm.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\rarvm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\rarvm.obj"	"$(INTDIR)\rarvm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\rijndael.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\rijndael.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\rijndael.obj"	"$(INTDIR)\rijndael.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\sha1.c

!IF  "$(CFG)" == "rar - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sha1.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sha1.obj"	"$(INTDIR)\sha1.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\base\stdflt.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\stdflt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\stdflt.obj"	"$(INTDIR)\stdflt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\suballoc.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\suballoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\suballoc.obj"	"$(INTDIR)\suballoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\unpack15.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\unpack15.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\unpack15.obj"	"$(INTDIR)\unpack15.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\unpack20.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\unpack20.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\unpack20.obj"	"$(INTDIR)\unpack20.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\base\unpack29.c

!IF  "$(CFG)" == "rar - Win32 Release"


"$(INTDIR)\unpack29.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "rar - Win32 Debug"


"$(INTDIR)\unpack29.obj"	"$(INTDIR)\unpack29.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 


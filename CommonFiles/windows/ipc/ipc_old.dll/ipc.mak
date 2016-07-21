# Microsoft Developer Studio Generated NMAKE File, Based on ipc.dsp
!IF "$(CFG)" == ""
CFG=ipc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ipc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ipc - Win32 Release" && "$(CFG)" != "ipc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ipc.mak" CFG="ipc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ipc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ipc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ipc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\release\ipc.dll" "$(OUTDIR)\ipc.bsc"


CLEAN :
	-@erase "$(INTDIR)\DateTime.obj"
	-@erase "$(INTDIR)\DateTime.sbr"
	-@erase "$(INTDIR)\HandlesTable.obj"
	-@erase "$(INTDIR)\HandlesTable.sbr"
	-@erase "$(INTDIR)\ipc.obj"
	-@erase "$(INTDIR)\ipc.res"
	-@erase "$(INTDIR)\ipc.sbr"
	-@erase "$(INTDIR)\ipc32.obj"
	-@erase "$(INTDIR)\ipc32.sbr"
	-@erase "$(INTDIR)\ipcBuffer.obj"
	-@erase "$(INTDIR)\ipcBuffer.sbr"
	-@erase "$(INTDIR)\ipcConnection.obj"
	-@erase "$(INTDIR)\ipcConnection.sbr"
	-@erase "$(INTDIR)\ipcInfo.obj"
	-@erase "$(INTDIR)\ipcInfo.sbr"
	-@erase "$(INTDIR)\ipcServer.obj"
	-@erase "$(INTDIR)\ipcServer.sbr"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sa.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ipc.bsc"
	-@erase "..\..\..\release\ipc.dll"
	-@erase "..\..\..\release\ipc.dll.map"
	-@erase "..\..\..\release\ipc.exp"
	-@erase "..\..\..\release\ipc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G4 /MD /W3 /GX /O1 /I "../../gcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ipc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ipc.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ipc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DateTime.sbr" \
	"$(INTDIR)\HandlesTable.sbr" \
	"$(INTDIR)\ipc.sbr" \
	"$(INTDIR)\ipc32.sbr" \
	"$(INTDIR)\ipcBuffer.sbr" \
	"$(INTDIR)\ipcConnection.sbr" \
	"$(INTDIR)\ipcInfo.sbr" \
	"$(INTDIR)\ipcServer.sbr" \
	"$(INTDIR)\sa.sbr"

"$(OUTDIR)\ipc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib /nologo /base:"0x62500000" /subsystem:windows /dll /incremental:no /pdb:"\release\ipc.dll.pdb" /map:"\release\ipc.dll.map" /machine:I386 /def:".\ipc.def" /out:"\release\ipc.dll" /implib:"\release\ipc.lib" /pdbtype:con /opt:ref 
DEF_FILE= \
	".\ipc.def"
LINK32_OBJS= \
	"$(INTDIR)\DateTime.obj" \
	"$(INTDIR)\HandlesTable.obj" \
	"$(INTDIR)\ipc.obj" \
	"$(INTDIR)\ipc32.obj" \
	"$(INTDIR)\ipcBuffer.obj" \
	"$(INTDIR)\ipcConnection.obj" \
	"$(INTDIR)\ipcInfo.obj" \
	"$(INTDIR)\ipcServer.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\ipc.res"

"..\..\..\release\ipc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ipc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\debug\ipc.dll" "$(OUTDIR)\ipc.bsc"


CLEAN :
	-@erase "$(INTDIR)\DateTime.obj"
	-@erase "$(INTDIR)\DateTime.sbr"
	-@erase "$(INTDIR)\HandlesTable.obj"
	-@erase "$(INTDIR)\HandlesTable.sbr"
	-@erase "$(INTDIR)\ipc.obj"
	-@erase "$(INTDIR)\ipc.res"
	-@erase "$(INTDIR)\ipc.sbr"
	-@erase "$(INTDIR)\ipc32.obj"
	-@erase "$(INTDIR)\ipc32.sbr"
	-@erase "$(INTDIR)\ipcBuffer.obj"
	-@erase "$(INTDIR)\ipcBuffer.sbr"
	-@erase "$(INTDIR)\ipcConnection.obj"
	-@erase "$(INTDIR)\ipcConnection.sbr"
	-@erase "$(INTDIR)\ipcInfo.obj"
	-@erase "$(INTDIR)\ipcInfo.sbr"
	-@erase "$(INTDIR)\ipcServer.obj"
	-@erase "$(INTDIR)\ipcServer.sbr"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sa.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ipc.bsc"
	-@erase "..\..\..\debug\ipc.dll"
	-@erase "..\..\..\debug\ipc.dll.pdb"
	-@erase "..\..\..\debug\ipc.exp"
	-@erase "..\..\..\debug\ipc.ilk"
	-@erase "..\..\..\debug\ipc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G4 /MDd /W3 /Gm /GX /Zi /Od /I "../../gcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ipc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ipc.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ipc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\DateTime.sbr" \
	"$(INTDIR)\HandlesTable.sbr" \
	"$(INTDIR)\ipc.sbr" \
	"$(INTDIR)\ipc32.sbr" \
	"$(INTDIR)\ipcBuffer.sbr" \
	"$(INTDIR)\ipcConnection.sbr" \
	"$(INTDIR)\ipcInfo.sbr" \
	"$(INTDIR)\ipcServer.sbr" \
	"$(INTDIR)\sa.sbr"

"$(OUTDIR)\ipc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib /nologo /base:"0x62500000" /subsystem:windows /dll /incremental:yes /pdb:"\debug\ipc.dll.pdb" /debug /machine:I386 /def:".\ipc.def" /out:"\debug\ipc.dll" /implib:"\debug\ipc.lib" /pdbtype:con 
DEF_FILE= \
	".\ipc.def"
LINK32_OBJS= \
	"$(INTDIR)\DateTime.obj" \
	"$(INTDIR)\HandlesTable.obj" \
	"$(INTDIR)\ipc.obj" \
	"$(INTDIR)\ipc32.obj" \
	"$(INTDIR)\ipcBuffer.obj" \
	"$(INTDIR)\ipcConnection.obj" \
	"$(INTDIR)\ipcInfo.obj" \
	"$(INTDIR)\ipcServer.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\ipc.res"

"..\..\..\debug\ipc.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ipc.dep")
!INCLUDE "ipc.dep"
!ELSE 
!MESSAGE Warning: cannot find "ipc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ipc - Win32 Release" || "$(CFG)" == "ipc - Win32 Debug"
SOURCE=..\..\gcommon\DateTime.cpp

"$(INTDIR)\DateTime.obj"	"$(INTDIR)\DateTime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\gcommon\HandlesTable.cpp

"$(INTDIR)\HandlesTable.obj"	"$(INTDIR)\HandlesTable.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ipc.cpp

"$(INTDIR)\ipc.obj"	"$(INTDIR)\ipc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ipc.rc

"$(INTDIR)\ipc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ipc32.cpp

"$(INTDIR)\ipc32.obj"	"$(INTDIR)\ipc32.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ipcBuffer.cpp

"$(INTDIR)\ipcBuffer.obj"	"$(INTDIR)\ipcBuffer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ipcConnection.cpp

"$(INTDIR)\ipcConnection.obj"	"$(INTDIR)\ipcConnection.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ipcInfo.cpp

"$(INTDIR)\ipcInfo.obj"	"$(INTDIR)\ipcInfo.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ipcServer.cpp

"$(INTDIR)\ipcServer.obj"	"$(INTDIR)\ipcServer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\gcommon\sa.cpp

"$(INTDIR)\sa.obj"	"$(INTDIR)\sa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on TRUE.dsp
!IF "$(CFG)" == ""
CFG=TRUE - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TRUE - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TRUE - Win32 Release" && "$(CFG)" != "TRUE - Win32 Debug" && "$(CFG)" != "TRUE - Win32 Test Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TRUE.mak" CFG="TRUE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TRUE - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TRUE - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TRUE - Win32 Test Release" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "TRUE - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\TRUE.lib"


CLEAN :
	-@erase "$(INTDIR)\ARP.obj"
	-@erase "$(INTDIR)\Eth.obj"
	-@erase "$(INTDIR)\FilterEvent.obj"
	-@erase "$(INTDIR)\Icmp.obj"
	-@erase "$(INTDIR)\IPv4.obj"
	-@erase "$(INTDIR)\NtBased.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Tcp.obj"
	-@erase "$(INTDIR)\Udp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TRUE.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /ML /W3 /GX /O2 /I "$(BASEDIR)\inc" /D "_MBCS" /D "_LIB" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D _DLL=1 /D _X86_=1 /Fp"$(INTDIR)\TRUE.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TRUE.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\TRUE.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ARP.obj" \
	"$(INTDIR)\Eth.obj" \
	"$(INTDIR)\Icmp.obj" \
	"$(INTDIR)\IPv4.obj" \
	"$(INTDIR)\Tcp.obj" \
	"$(INTDIR)\Udp.obj" \
	"$(INTDIR)\FilterEvent.obj" \
	"$(INTDIR)\NtBased.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\TRUE.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\TRUE.lib"
   PostBuild.cmd Release
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\TRUE.lib" "$(OUTDIR)\TRUE.bsc"


CLEAN :
	-@erase "$(INTDIR)\ARP.obj"
	-@erase "$(INTDIR)\ARP.sbr"
	-@erase "$(INTDIR)\Eth.obj"
	-@erase "$(INTDIR)\Eth.sbr"
	-@erase "$(INTDIR)\FilterEvent.obj"
	-@erase "$(INTDIR)\FilterEvent.sbr"
	-@erase "$(INTDIR)\Icmp.obj"
	-@erase "$(INTDIR)\Icmp.sbr"
	-@erase "$(INTDIR)\IPv4.obj"
	-@erase "$(INTDIR)\IPv4.sbr"
	-@erase "$(INTDIR)\NtBased.obj"
	-@erase "$(INTDIR)\NtBased.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\Tcp.obj"
	-@erase "$(INTDIR)\Tcp.sbr"
	-@erase "$(INTDIR)\Udp.obj"
	-@erase "$(INTDIR)\Udp.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TRUE.bsc"
	-@erase "$(OUTDIR)\TRUE.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MTd /W3 /Zi /Od /Gy /I "$(BASEDIR2k)\inc" /I "$(BASEDIR2k)\inc\ddk" /I "$(CPU)\\" /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_LIB" /D "_DEBUG" /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D _DLL=1 /D _X86_=1 /D _DBG_INFO_LEVEL=$(INFODEBUGLEVEL) /D _AVPG_UNLOADABLE=$(AVPG_UNLOADABLE) /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /Zel -cbstring /QIfdiv- /QIf /GF /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TRUE.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ARP.sbr" \
	"$(INTDIR)\Eth.sbr" \
	"$(INTDIR)\Icmp.sbr" \
	"$(INTDIR)\IPv4.sbr" \
	"$(INTDIR)\Tcp.sbr" \
	"$(INTDIR)\Udp.sbr" \
	"$(INTDIR)\FilterEvent.sbr" \
	"$(INTDIR)\NtBased.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\TRUE.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\TRUE.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ARP.obj" \
	"$(INTDIR)\Eth.obj" \
	"$(INTDIR)\Icmp.obj" \
	"$(INTDIR)\IPv4.obj" \
	"$(INTDIR)\Tcp.obj" \
	"$(INTDIR)\Udp.obj" \
	"$(INTDIR)\FilterEvent.obj" \
	"$(INTDIR)\NtBased.obj" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\TRUE.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\TRUE.lib" "$(OUTDIR)\TRUE.bsc"
   PostBuild.cmd Debug
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"

OUTDIR=.\TRUE___Win32_Test_Release
INTDIR=.\TRUE___Win32_Test_Release

ALL : "..\Release\TRUE.lib"


CLEAN :
	-@erase "$(INTDIR)\ARP.obj"
	-@erase "$(INTDIR)\Eth.obj"
	-@erase "$(INTDIR)\FilterEvent.obj"
	-@erase "$(INTDIR)\Icmp.obj"
	-@erase "$(INTDIR)\IPv4.obj"
	-@erase "$(INTDIR)\NtBased.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\Tcp.obj"
	-@erase "$(INTDIR)\Udp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\Release\TRUE.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "$(BASEDIR2k)\inc" /I "$(BASEDIR2k)\inc\ddk" /I "$(CPU)\\" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D _DLL=1 /D _X86_=1 /D "NDIS40" /D "NDIS_MINIPORT_DRIVER" /D "NDIS40_MINIPORT" /D "NDIS_WDM" /D _DBG_INFO_LEVEL=$(INFODEBUGLEVEL) /D "_TEST_RELEASE" /Fp"$(INTDIR)\TRUE.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TRUE.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\Release\TRUE.lib" 
LIB32_OBJS= \
	"$(INTDIR)\ARP.obj" \
	"$(INTDIR)\Eth.obj" \
	"$(INTDIR)\Icmp.obj" \
	"$(INTDIR)\IPv4.obj" \
	"$(INTDIR)\Tcp.obj" \
	"$(INTDIR)\Udp.obj" \
	"$(INTDIR)\FilterEvent.obj" \
	"$(INTDIR)\NtBased.obj" \
	"$(INTDIR)\StdAfx.obj"

"..\Release\TRUE.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("TRUE.dep")
!INCLUDE "TRUE.dep"
!ELSE 
!MESSAGE Warning: cannot find "TRUE.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TRUE - Win32 Release" || "$(CFG)" == "TRUE - Win32 Debug" || "$(CFG)" == "TRUE - Win32 Test Release"
SOURCE=.\Protocols\ARP.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\ARP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\ARP.obj"	"$(INTDIR)\ARP.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\ARP.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Protocols\Eth.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\Eth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\Eth.obj"	"$(INTDIR)\Eth.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\Eth.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Protocols\Icmp.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\Icmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\Icmp.obj"	"$(INTDIR)\Icmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\Icmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Protocols\IPv4.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\IPv4.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\IPv4.obj"	"$(INTDIR)\IPv4.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\IPv4.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Protocols\Tcp.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\Tcp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\Tcp.obj"	"$(INTDIR)\Tcp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\Tcp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Protocols\Udp.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\Udp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\Udp.obj"	"$(INTDIR)\Udp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\Udp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\FilterEvent.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\FilterEvent.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\FilterEvent.obj"	"$(INTDIR)\FilterEvent.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\FilterEvent.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\NtBased.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\NtBased.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\NtBased.obj"	"$(INTDIR)\NtBased.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\NtBased.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\StdAfx.c

!IF  "$(CFG)" == "TRUE - Win32 Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TRUE - Win32 Debug"


"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TRUE - Win32 Test Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 


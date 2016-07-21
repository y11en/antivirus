# Microsoft Developer Studio Generated NMAKE File, Based on Kernel.dsp
!IF "$(CFG)" == ""
CFG=Kernel - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Kernel - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Kernel - Win32 Release" && "$(CFG)" != "Kernel - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Kernel.mak" CFG="Kernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Kernel - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Kernel - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Kernel - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/Kernel
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PrKernel.ppl"

!ELSE 

ALL : "stream - Win32 Release" "_CommonFiles_NoUnicode - Win32 Release" "$(OUTDIR)\PrKernel.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"_CommonFiles_NoUnicode - Win32 ReleaseCLEAN" "stream - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\custprop.obj"
	-@erase "$(INTDIR)\dt.obj"
	-@erase "$(INTDIR)\handle.obj"
	-@erase "$(INTDIR)\iface.obj"
	-@erase "$(INTDIR)\impex.obj"
	-@erase "$(INTDIR)\k_ifenum.obj"
	-@erase "$(INTDIR)\k_root.obj"
	-@erase "$(INTDIR)\k_trace.obj"
	-@erase "$(INTDIR)\kernel.obj"
	-@erase "$(INTDIR)\kernel.res"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\mh_array.obj"
	-@erase "$(INTDIR)\not_sink.obj"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\prop.obj"
	-@erase "$(INTDIR)\prop_str.obj"
	-@erase "$(INTDIR)\receiver.obj"
	-@erase "$(INTDIR)\ser_reg.obj"
	-@erase "$(INTDIR)\ser_stream.obj"
	-@erase "$(INTDIR)\serializer.obj"
	-@erase "$(INTDIR)\synchro.obj"
	-@erase "$(INTDIR)\sys_hand.obj"
	-@erase "$(INTDIR)\sys_mem.obj"
	-@erase "$(INTDIR)\sys_msg.obj"
	-@erase "$(INTDIR)\sys_obj.obj"
	-@erase "$(INTDIR)\sys_prop.obj"
	-@erase "$(INTDIR)\sys_sync.obj"
	-@erase "$(INTDIR)\sys_trc.obj"
	-@erase "$(INTDIR)\system.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wrappers.obj"
	-@erase "$(OUTDIR)\PrKernel.exp"
	-@erase "$(OUTDIR)\PrKernel.pdb"
	-@erase "$(OUTDIR)\PrKernel.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O2 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\kernel.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Kernel.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib hashutil.lib stream.lib /nologo /base:"0x64a00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\PrKernel.pdb" /debug /machine:I386 /out:"$(OUTDIR)\PrKernel.ppl" /implib:"$(OUTDIR)\PrKernel.lib" /pdbtype:sept /libpath:"../../out/release" /IGNORE:4086 /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\custprop.obj" \
	"$(INTDIR)\dt.obj" \
	"$(INTDIR)\handle.obj" \
	"$(INTDIR)\iface.obj" \
	"$(INTDIR)\impex.obj" \
	"$(INTDIR)\k_ifenum.obj" \
	"$(INTDIR)\k_root.obj" \
	"$(INTDIR)\k_trace.obj" \
	"$(INTDIR)\kernel.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\mh_array.obj" \
	"$(INTDIR)\not_sink.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\prop.obj" \
	"$(INTDIR)\prop_str.obj" \
	"$(INTDIR)\receiver.obj" \
	"$(INTDIR)\ser_reg.obj" \
	"$(INTDIR)\ser_stream.obj" \
	"$(INTDIR)\serializer.obj" \
	"$(INTDIR)\synchro.obj" \
	"$(INTDIR)\sys_hand.obj" \
	"$(INTDIR)\sys_mem.obj" \
	"$(INTDIR)\sys_msg.obj" \
	"$(INTDIR)\sys_obj.obj" \
	"$(INTDIR)\sys_prop.obj" \
	"$(INTDIR)\sys_sync.obj" \
	"$(INTDIR)\sys_trc.obj" \
	"$(INTDIR)\system.obj" \
	"$(INTDIR)\wrappers.obj" \
	"$(INTDIR)\kernel.res" \
	"$(OUTDIR)\stream.lib"

"$(OUTDIR)\PrKernel.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\PrKernel.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "stream - Win32 Release" "_CommonFiles_NoUnicode - Win32 Release" "$(OUTDIR)\PrKernel.ppl"
   call prconvert "\out\Release\PrKernel.ppl"
	tsigner "\out\Release\PrKernel.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/Kernel
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\PrKernel.ppl" "$(OUTDIR)\Kernel.bsc"

!ELSE 

ALL : "stream - Win32 Debug" "_CommonFiles_NoUnicode - Win32 Debug" "$(OUTDIR)\PrKernel.ppl" "$(OUTDIR)\Kernel.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"_CommonFiles_NoUnicode - Win32 DebugCLEAN" "stream - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\custprop.obj"
	-@erase "$(INTDIR)\custprop.sbr"
	-@erase "$(INTDIR)\dt.obj"
	-@erase "$(INTDIR)\dt.sbr"
	-@erase "$(INTDIR)\handle.obj"
	-@erase "$(INTDIR)\handle.sbr"
	-@erase "$(INTDIR)\iface.obj"
	-@erase "$(INTDIR)\iface.sbr"
	-@erase "$(INTDIR)\impex.obj"
	-@erase "$(INTDIR)\impex.sbr"
	-@erase "$(INTDIR)\k_ifenum.obj"
	-@erase "$(INTDIR)\k_ifenum.sbr"
	-@erase "$(INTDIR)\k_root.obj"
	-@erase "$(INTDIR)\k_root.sbr"
	-@erase "$(INTDIR)\k_trace.obj"
	-@erase "$(INTDIR)\k_trace.sbr"
	-@erase "$(INTDIR)\kernel.obj"
	-@erase "$(INTDIR)\kernel.res"
	-@erase "$(INTDIR)\kernel.sbr"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\memory.sbr"
	-@erase "$(INTDIR)\mh_array.obj"
	-@erase "$(INTDIR)\mh_array.sbr"
	-@erase "$(INTDIR)\not_sink.obj"
	-@erase "$(INTDIR)\not_sink.sbr"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\output.sbr"
	-@erase "$(INTDIR)\prop.obj"
	-@erase "$(INTDIR)\prop.sbr"
	-@erase "$(INTDIR)\prop_str.obj"
	-@erase "$(INTDIR)\prop_str.sbr"
	-@erase "$(INTDIR)\receiver.obj"
	-@erase "$(INTDIR)\receiver.sbr"
	-@erase "$(INTDIR)\ser_reg.obj"
	-@erase "$(INTDIR)\ser_reg.sbr"
	-@erase "$(INTDIR)\ser_stream.obj"
	-@erase "$(INTDIR)\ser_stream.sbr"
	-@erase "$(INTDIR)\serializer.obj"
	-@erase "$(INTDIR)\serializer.sbr"
	-@erase "$(INTDIR)\synchro.obj"
	-@erase "$(INTDIR)\synchro.sbr"
	-@erase "$(INTDIR)\sys_hand.obj"
	-@erase "$(INTDIR)\sys_hand.sbr"
	-@erase "$(INTDIR)\sys_mem.obj"
	-@erase "$(INTDIR)\sys_mem.sbr"
	-@erase "$(INTDIR)\sys_msg.obj"
	-@erase "$(INTDIR)\sys_msg.sbr"
	-@erase "$(INTDIR)\sys_obj.obj"
	-@erase "$(INTDIR)\sys_obj.sbr"
	-@erase "$(INTDIR)\sys_prop.obj"
	-@erase "$(INTDIR)\sys_prop.sbr"
	-@erase "$(INTDIR)\sys_sync.obj"
	-@erase "$(INTDIR)\sys_sync.sbr"
	-@erase "$(INTDIR)\sys_trc.obj"
	-@erase "$(INTDIR)\sys_trc.sbr"
	-@erase "$(INTDIR)\system.obj"
	-@erase "$(INTDIR)\system.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wrappers.obj"
	-@erase "$(INTDIR)\wrappers.sbr"
	-@erase "$(OUTDIR)\Kernel.bsc"
	-@erase "$(OUTDIR)\PrKernel.exp"
	-@erase "$(OUTDIR)\PrKernel.ilk"
	-@erase "$(OUTDIR)\PrKernel.map"
	-@erase "$(OUTDIR)\PrKernel.pdb"
	-@erase "$(OUTDIR)\PrKernel.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\include" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\kernel.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Kernel.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\custprop.sbr" \
	"$(INTDIR)\dt.sbr" \
	"$(INTDIR)\handle.sbr" \
	"$(INTDIR)\iface.sbr" \
	"$(INTDIR)\impex.sbr" \
	"$(INTDIR)\k_ifenum.sbr" \
	"$(INTDIR)\k_root.sbr" \
	"$(INTDIR)\k_trace.sbr" \
	"$(INTDIR)\kernel.sbr" \
	"$(INTDIR)\memory.sbr" \
	"$(INTDIR)\mh_array.sbr" \
	"$(INTDIR)\not_sink.sbr" \
	"$(INTDIR)\output.sbr" \
	"$(INTDIR)\prop.sbr" \
	"$(INTDIR)\prop_str.sbr" \
	"$(INTDIR)\receiver.sbr" \
	"$(INTDIR)\ser_reg.sbr" \
	"$(INTDIR)\ser_stream.sbr" \
	"$(INTDIR)\serializer.sbr" \
	"$(INTDIR)\synchro.sbr" \
	"$(INTDIR)\sys_hand.sbr" \
	"$(INTDIR)\sys_mem.sbr" \
	"$(INTDIR)\sys_msg.sbr" \
	"$(INTDIR)\sys_obj.sbr" \
	"$(INTDIR)\sys_prop.sbr" \
	"$(INTDIR)\sys_sync.sbr" \
	"$(INTDIR)\sys_trc.sbr" \
	"$(INTDIR)\system.sbr" \
	"$(INTDIR)\wrappers.sbr"

"$(OUTDIR)\Kernel.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=m_utils.lib hashutil.lib stream.lib /nologo /entry:"DllMain" /dll /incremental:yes /pdb:"$(OUTDIR)\PrKernel.pdb" /map:"../../out/Debug/PrKernel.map" /debug /machine:I386 /out:"$(OUTDIR)\PrKernel.ppl" /implib:"$(OUTDIR)\PrKernel.lib" /pdbtype:sept /libpath:"../../out/debug" /ignore:4086 /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\custprop.obj" \
	"$(INTDIR)\dt.obj" \
	"$(INTDIR)\handle.obj" \
	"$(INTDIR)\iface.obj" \
	"$(INTDIR)\impex.obj" \
	"$(INTDIR)\k_ifenum.obj" \
	"$(INTDIR)\k_root.obj" \
	"$(INTDIR)\k_trace.obj" \
	"$(INTDIR)\kernel.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\mh_array.obj" \
	"$(INTDIR)\not_sink.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\prop.obj" \
	"$(INTDIR)\prop_str.obj" \
	"$(INTDIR)\receiver.obj" \
	"$(INTDIR)\ser_reg.obj" \
	"$(INTDIR)\ser_stream.obj" \
	"$(INTDIR)\serializer.obj" \
	"$(INTDIR)\synchro.obj" \
	"$(INTDIR)\sys_hand.obj" \
	"$(INTDIR)\sys_mem.obj" \
	"$(INTDIR)\sys_msg.obj" \
	"$(INTDIR)\sys_obj.obj" \
	"$(INTDIR)\sys_prop.obj" \
	"$(INTDIR)\sys_sync.obj" \
	"$(INTDIR)\sys_trc.obj" \
	"$(INTDIR)\system.obj" \
	"$(INTDIR)\wrappers.obj" \
	"$(INTDIR)\kernel.res" \
	"$(OUTDIR)\stream.lib"

"$(OUTDIR)\PrKernel.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Kernel.dep")
!INCLUDE "Kernel.dep"
!ELSE 
!MESSAGE Warning: cannot find "Kernel.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Kernel - Win32 Release" || "$(CFG)" == "Kernel - Win32 Debug"
SOURCE=.\custprop.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\custprop.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\custprop.obj"	"$(INTDIR)\custprop.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\time\dt.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\dt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\dt.obj"	"$(INTDIR)\dt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\handle.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\handle.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\handle.obj"	"$(INTDIR)\handle.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\iface.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\iface.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\iface.obj"	"$(INTDIR)\iface.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\impex.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\impex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\impex.obj"	"$(INTDIR)\impex.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\k_ifenum.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\k_ifenum.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\k_ifenum.obj"	"$(INTDIR)\k_ifenum.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\k_root.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\k_root.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\k_root.obj"	"$(INTDIR)\k_root.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\k_trace.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\k_trace.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\k_trace.obj"	"$(INTDIR)\k_trace.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\kernel.c

!IF  "$(CFG)" == "Kernel - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /Zi /O2 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KERNEL_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kernel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /ZI /Od /I "..\include" /I "..\..\CommonFiles" /I "..\..\ppp\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kernel.obj"	"$(INTDIR)\kernel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\memory.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\memory.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\memory.obj"	"$(INTDIR)\memory.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mh_array.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\mh_array.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\mh_array.obj"	"$(INTDIR)\mh_array.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\not_sink.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\not_sink.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\not_sink.obj"	"$(INTDIR)\not_sink.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\output.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\output.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\output.obj"	"$(INTDIR)\output.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\prop.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\prop.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\prop.obj"	"$(INTDIR)\prop.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\prop_str.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\prop_str.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\prop_str.obj"	"$(INTDIR)\prop_str.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\receiver.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\receiver.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\receiver.obj"	"$(INTDIR)\receiver.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ser_reg.cpp

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\ser_reg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\ser_reg.obj"	"$(INTDIR)\ser_reg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ser_stream.cpp

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\ser_stream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\ser_stream.obj"	"$(INTDIR)\ser_stream.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\serializer.cpp

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\serializer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\serializer.obj"	"$(INTDIR)\serializer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\synchro.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\synchro.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\synchro.obj"	"$(INTDIR)\synchro.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_hand.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_hand.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_hand.obj"	"$(INTDIR)\sys_hand.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_mem.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_mem.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_mem.obj"	"$(INTDIR)\sys_mem.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_msg.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_msg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_msg.obj"	"$(INTDIR)\sys_msg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_obj.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_obj.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_obj.obj"	"$(INTDIR)\sys_obj.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_prop.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_prop.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_prop.obj"	"$(INTDIR)\sys_prop.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_sync.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_sync.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_sync.obj"	"$(INTDIR)\sys_sync.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\sys_trc.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\sys_trc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\sys_trc.obj"	"$(INTDIR)\sys_trc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\system.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\system.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\system.obj"	"$(INTDIR)\system.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\wrappers.c

!IF  "$(CFG)" == "Kernel - Win32 Release"


"$(INTDIR)\wrappers.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"


"$(INTDIR)\wrappers.obj"	"$(INTDIR)\wrappers.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\kernel.rc

"$(INTDIR)\kernel.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Kernel - Win32 Release"

"_CommonFiles_NoUnicode - Win32 Release" : 
   cd "\CommonFiles"
   NMAKE /f _msdev_make.mak TARGET="All - Win32 ReleaseDll" DSW="CommonFiles"
   cd "..\prague\kernel"

"_CommonFiles_NoUnicode - Win32 ReleaseCLEAN" : 
   cd "\CommonFiles"
   cd "..\prague\kernel"

!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"

"_CommonFiles_NoUnicode - Win32 Debug" : 
   cd "\CommonFiles"
   NMAKE /f _msdev_make.mak TARGET="All - Win32 DebugDll" DSW="CommonFiles"
   cd "..\prague\kernel"

"_CommonFiles_NoUnicode - Win32 DebugCLEAN" : 
   cd "\CommonFiles"
   cd "..\prague\kernel"

!ENDIF 

!IF  "$(CFG)" == "Kernel - Win32 Release"

"stream - Win32 Release" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" 
   cd "..\kernel"

"stream - Win32 ReleaseCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" RECURSE=1 CLEAN 
   cd "..\kernel"

!ELSEIF  "$(CFG)" == "Kernel - Win32 Debug"

"stream - Win32 Debug" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" 
   cd "..\kernel"

"stream - Win32 DebugCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\kernel"

!ENDIF 


!ENDIF 


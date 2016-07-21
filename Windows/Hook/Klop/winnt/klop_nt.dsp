# Microsoft Developer Studio Project File - Name="Klop_nt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=klop_nt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klop_nt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klop_nt.mak" CFG="klop_nt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Klop_nt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Klop_nt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Klop/winnt", OIZMCAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../temp/release/Klop_nt"
# PROP Intermediate_Dir "../../../../../temp/release/Klop_nt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /D "WIN32" /Oxs /c
# ADD CPP /nologo /Gz /W3 /Zi /Oy /Gy /I "..\..\\" /I "..\..\klsdk" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /FI"$(BASEDIR)\inc\warning.h" /D "__CPP" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /FAs /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib tdi.lib ndis.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"../../../../../out/release/Klop.pdb" /machine:IX86 /nodefaultlib /out:"../../../../../out/release/Klop.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"..\..\Release\\" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none /debug
# Begin Special Build Tool
TargetPath=\Perforce\out\release\Klop.sys
SOURCE="$(InputPath)"
PostBuild_Cmds=call kl1sign "$(TargetPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../temp/debug/Klop_nt"
# PROP Intermediate_Dir "../../../../../temp/debug/Klop_nt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "..\..\KLSDK" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\\" /I "..\..\klsdk" /FI"$(BASEDIR)\inc\warning.h" /D "__CPP" /D "_DEBUG" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "HOOK_MODE_LOAD" /FAs /FR /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\..\..\..\..\CommonFiles"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386\IDS00000.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib ndis.lib $(BASEDIR2k)\libchk\i386\ndis.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"../../../../../out/debug/Klop.pdb" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"../../../../../out/debug/Klop.sys" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"..\..\debug" /driver /debug:notmapped,FULL /IGNORE:4006,4001,4037,4039,4065,4078,4087,4088,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Special Build Tool
TargetPath=\Perforce\out\debug\Klop.sys
SOURCE="$(InputPath)"
PostBuild_Cmds=call kl1sign "$(TargetPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Klop_nt - Win32 Release"
# Name "Klop_nt - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\g_cp_proto.cpp
DEP_CPP_G_CP_=\
	"..\..\HOOK\klop_api.h"\
	"..\..\klned\Ids_common.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\g_cp_proto.h"\
	"..\g_custom_packet.h"\
	"..\g_key.h"\
	"..\g_klop_ndis.h"\
	"..\klopevent.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_G_CP_=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\g_custom_packet.cpp
DEP_CPP_G_CUS=\
	"..\..\HOOK\klop_api.h"\
	"..\..\klned\Ids_common.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\g_cp_proto.h"\
	"..\g_custom_packet.h"\
	"..\g_key.h"\
	"..\g_klop_ndis.h"\
	"..\klopevent.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_G_CUS=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\g_dispatcher.cpp
DEP_CPP_G_DIS=\
	"..\..\HOOK\klop_api.h"\
	"..\..\klned\Ids_common.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\g_cp_proto.h"\
	"..\g_custom_packet.h"\
	"..\g_dispatcher.h"\
	"..\g_key.h"\
	"..\g_klop_ndis.h"\
	"..\klopevent.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_G_DIS=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\g_key.cpp
DEP_CPP_G_KEY=\
	"..\..\HOOK\klop_api.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\md5.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\g_key.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_G_KEY=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\g_klop_ndis.cpp
DEP_CPP_G_KLO=\
	"..\..\HOOK\klop_api.h"\
	"..\..\klned\Ids_common.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\g_cp_proto.h"\
	"..\g_custom_packet.h"\
	"..\g_key.h"\
	"..\g_klop_ndis.h"\
	"..\klopevent.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_G_KLO=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KL1\klload.cpp
DEP_CPP_KLLOA=\
	"..\..\KL1\klload.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KLLOA=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\klopevent.cpp
DEP_CPP_KLOPE=\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\klopevent.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KLOPE=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp
DEP_CPP_MAIN_=\
	"..\..\HOOK\klop_api.h"\
	"..\..\KL1\klload.h"\
	"..\..\klned\Ids_common.h"\
	"..\..\klsdk\kl_hook.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kl_registry.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\klsdk\pe.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\g_cp_proto.h"\
	"..\g_custom_packet.h"\
	"..\g_dispatcher.h"\
	"..\g_key.h"\
	"..\g_klop_ndis.h"\
	"..\klopevent.h"\
	".\main.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_MAIN_=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\md5.cpp
DEP_CPP_MD5_C=\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\md5.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_MD5_C=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\g_cp_proto.h
# End Source File
# Begin Source File

SOURCE=..\g_custom_packet.h
# End Source File
# Begin Source File

SOURCE=..\g_dispatcher.h
# End Source File
# Begin Source File

SOURCE=..\g_key.h
# End Source File
# Begin Source File

SOURCE=..\g_klop_ndis.h
# End Source File
# Begin Source File

SOURCE=..\..\KL1\klload.h
# End Source File
# Begin Source File

SOURCE=..\..\HOOK\klop_api.h
# End Source File
# Begin Source File

SOURCE=..\klopevent.h
# End Source File
# End Group
# Begin Group "KLSDK source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\KLSDK\kl_hook.cpp
DEP_CPP_KL_HO=\
	"..\..\klsdk\kl_hook.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\klsdk\pe.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_HO=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_lock.cpp
DEP_CPP_KL_LO=\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_LO=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_object.cpp
DEP_CPP_KL_OB=\
	"..\..\KLSDK\kl_debug.h"\
	"..\..\KLSDK\kl_DebugMask.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_OB=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_registry.cpp
DEP_CPP_KL_RE=\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kl_registry.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\klsdk\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_RE=\
	"..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "Klop_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "Klop_nt - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "KLSDK include"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=..\changes.txt
# End Source File
# Begin Source File

SOURCE=.\klop_nt.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="logger" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=logger - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "loggernt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "loggernt.mak" CFG="logger - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "logger - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "logger - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Plugins/logger/winnt", QQMOAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "logger - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../../temp/release/logger"
# PROP Intermediate_Dir "../../../../../temp/release/logger"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /D "WIN32" /Oxs /c
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\..\KLSDK" /I "..\..\.." /I "..\..\..\..\..\Components\Windows\KAH\IDS" /I "..\..\..\..\..\klava\kernel\include" /I "..\..\..\..\..\prague\include" /FI"$(BASEDIR)\inc\warning.h" /D "__CPP" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib tdi.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"../../../../../out/release/logger.pdb" /machine:IX86 /nodefaultlib /out:"../../../../../out/release/logger.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"..\..\Release\\" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../../temp/debug/logger"
# PROP Intermediate_Dir "../../../../../temp/debug/logger"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\..\KLSDK" /I "..\..\.." /I "..\..\..\..\..\Components\Windows\KAH\IDS" /I "..\..\..\..\..\klava\kernel\include" /I "..\..\..\..\..\prague\include" /FI"$(BASEDIR)\inc\warning.h" /D "__CPP" /D "_DEBUG" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "HOOK_MODE_LOAD" /FR /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386\logger.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib tdi.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"../../../../../out/debug/logger.pdb" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"../../../../../out/debug/logger.sys" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"..\..\debug" /driver /debug:notmapped,FULL /IGNORE:4006,4001,4037,4039,4065,4078,4087,4088,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "logger - Win32 Release"
# Name "logger - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\KL1\klload.cpp
DEP_CPP_KLLOA=\
	"..\..\..\kl1\klload.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KLLOA=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\logger.cpp
DEP_CPP_LOGGE=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\klick_api.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\Klick\g_reg_base.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\klned\ids_common.h"\
	"..\..\..\KLNED\kl_filter.h"\
	"..\..\..\KLNED\klickplugin.h"\
	"..\..\..\KLSDK\g_thread.h"\
	"..\..\..\KLSDK\kl_buffer.h"\
	"..\..\..\KLSDK\kl_file.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_log.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\PlugAPI.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"..\..\Include\logger_api.h"\
	"..\descriptions.h"\
	"..\logger.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_LOGGE=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Loggermod.cpp
DEP_CPP_LOGGER=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\klick_api.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\Klick\g_reg_base.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\klned\ids_common.h"\
	"..\..\..\KLNED\kl_filter.h"\
	"..\..\..\KLNED\kl_plugmod.h"\
	"..\..\..\KLNED\klickplugin.h"\
	"..\..\..\KLSDK\g_thread.h"\
	"..\..\..\KLSDK\kl_buffer.h"\
	"..\..\..\KLSDK\kl_file.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_log.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\PlugAPI.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"..\..\Include\logger_api.h"\
	"..\g_precomp.h"\
	"..\logger.h"\
	"..\loggermod.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_LOGGER=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp
DEP_CPP_MAIN_=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\klick_api.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\kl1\klload.h"\
	"..\..\..\Klick\g_reg_base.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\klned\ids_common.h"\
	"..\..\..\KLNED\kl_filter.h"\
	"..\..\..\KLNED\kl_plugmod.h"\
	"..\..\..\KLNED\klickplugin.h"\
	"..\..\..\KLSDK\g_thread.h"\
	"..\..\..\KLSDK\kl_buffer.h"\
	"..\..\..\KLSDK\kl_file.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_log.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\PlugAPI.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"..\..\Include\logger_api.h"\
	"..\..\nt_plug_starter.h"\
	"..\g_precomp.h"\
	"..\logger.h"\
	"..\loggermod.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_MAIN_=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\g_precomp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\Ids_common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\kl1\klload.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\logger_api.h
# End Source File
# Begin Source File

SOURCE=..\..\nt_plug_starter.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\PlugAPI.h
# End Source File
# End Group
# Begin Group "KLSDK source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\KLSDK\g_thread.cpp
DEP_CPP_G_THR=\
	"..\..\..\KLSDK\g_thread.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_G_THR=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_buffer.cpp
DEP_CPP_KL_BU=\
	"..\..\..\KLSDK\kl_buffer.h"\
	"..\..\..\KLSDK\kl_file.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_BU=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_file.cpp
DEP_CPP_KL_FI=\
	"..\..\..\KLSDK\kl_file.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_FI=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_list.cpp
DEP_CPP_KL_LI=\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_LI=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_lock.cpp
DEP_CPP_KL_LO=\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_LO=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_log.cpp
DEP_CPP_KL_LOG=\
	"..\..\..\KLSDK\g_thread.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_log.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_LOG=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_object.cpp
DEP_CPP_KL_OB=\
	"..\..\..\KLSDK\kl_debug.h"\
	"..\..\..\KLSDK\kl_DebugMask.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_OB=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_string.cpp
DEP_CPP_KL_ST=\
	"..\..\..\KLSDK\kl_buffer.h"\
	"..\..\..\KLSDK\kl_file.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kl_string.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_ST=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "KLSDK include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\KLSDK\g_thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_list.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_lock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\KLSDK\kl_string.h
# End Source File
# End Group
# Begin Group "KLNED source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\KLNED\datapacket.cpp
DEP_CPP_DATAP=\
	"..\..\..\hook\klick_api.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\KLNED\datapacket.h"\
	"..\..\..\klned\ids_common.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_DATAP=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLNED\kl_Filter.cpp
DEP_CPP_KL_FIL=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\KLNED\kl_filter.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\PlugAPI.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_FIL=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLNED\kl_plugmod.cpp
DEP_CPP_KL_PL=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\KLNED\kl_filter.h"\
	"..\..\..\KLNED\kl_plugmod.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\PlugAPI.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KL_PL=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\KLNED\klickplugin.cpp
DEP_CPP_KLICK=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\klick_api.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\Klick\g_reg_base.h"\
	"..\..\..\klned\baseapi.h"\
	"..\..\..\klned\ids_common.h"\
	"..\..\..\KLNED\kl_filter.h"\
	"..\..\..\KLNED\klickplugin.h"\
	"..\..\..\KLSDK\kl_list.h"\
	"..\..\..\KLSDK\kl_lock.h"\
	"..\..\..\KLSDK\kl_object.h"\
	"..\..\..\KLSDK\kldef.h"\
	"..\..\..\KLSDK\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\KLSDK\nt_def.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\KLSDK\PlugAPI.h"\
	"..\..\..\KLSDK\ring3def.h"\
	"..\..\..\klsdk\tdi.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"c:\work\ddk\nt40\inc\alpharef.h"\
	"c:\work\ddk\nt40\inc\bugcodes.h"\
	"c:\work\ddk\nt40\inc\exlevels.h"\
	"c:\work\ddk\nt40\inc\ntddk.h"\
	"c:\work\ddk\nt40\inc\ntdef.h"\
	"c:\work\ddk\nt40\inc\ntiologc.h"\
	"c:\work\ddk\nt40\inc\ntpoapi.h"\
	"c:\work\ddk\nt40\inc\ntstatus.h"\
	"c:\work\ddk\nt40\src\network\inc\afilter.h"\
	"c:\work\ddk\nt40\src\network\inc\efilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ffilter.h"\
	"c:\work\ddk\nt40\src\network\inc\ndis.h"\
	"c:\work\ddk\nt40\src\network\inc\netevent.h"\
	"c:\work\ddk\nt40\src\network\inc\nettypes.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddndis.h"\
	"c:\work\ddk\nt40\src\network\inc\ntddtdi.h"\
	"c:\work\ddk\nt40\src\network\inc\packoff.h"\
	"c:\work\ddk\nt40\src\network\inc\packon.h"\
	"c:\work\ddk\nt40\src\network\inc\tdi.h"\
	"c:\work\ddk\nt40\src\network\inc\tdikrnl.h"\
	"c:\work\ddk\nt40\src\network\inc\tfilter.h"\
	
NODEP_CPP_KLICK=\
	"..\..\..\KLSDK\tdistat.h"\
	

!IF  "$(CFG)" == "logger - Win32 Release"

!ELSEIF  "$(CFG)" == "logger - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "KLNED include"

# PROP Default_Filter ""
# End Group
# End Target
# End Project

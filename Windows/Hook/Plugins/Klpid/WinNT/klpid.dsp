# Microsoft Developer Studio Project File - Name="klpid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=klpid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "klpid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "klpid.mak" CFG="klpid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "klpid - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "klpid - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/Plugins/Klpid", GQYHAAAA"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /D "WIN32" /Oxs /c
# ADD CPP /nologo /Gz /W3 /Oy /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\..\KLSDK" /FI"$(BASEDIR)\inc\warning.h" /D "_UNICODE" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "HOOK_MODE_LOAD" /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib ndis.lib true.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /machine:IX86 /nodefaultlib /out:"Release\klpid.sys" /libpath:"$(BASEDIR2K)\libfre\i386" /libpath:"..\..\..\Release" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\..\KLSDK" /FI"$(BASEDIR)\inc\warning.h" /D "_UNICODE" /D "_DEBUG" /D WIN32=100 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0400 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "HOOK_MODE_LOAD" /FR /YX /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386\klpid.bsc"
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib ndis.lib true.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"debug\klpid.sys" /libpath:"$(BASEDIR2K)\libchk\i386" /libpath:"..\..\..\Debug" /driver /debug:notmapped,FULL /IGNORE:4006,4001,4037,4039,4065,4078,4087,4088,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
TargetPath=.\debug\klpid.sys
TargetName=klpid
SOURCE="$(InputPath)"
PostBuild_Cmds=PostBuild.cmd Debug $(TargetPath) $(TargetName)
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "klpid - Win32 Release"
# Name "klpid - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Group "IDS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\alert.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\attacks.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\attacks.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\dos.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Element.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Flood.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Parse.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Scan.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\trust.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Stealth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\stealth.c

!IF  "$(CFG)" == "klpid - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\STEALTH.H
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=..\handlers.c
DEP_CPP_HANDL=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\netid.h"\
	"..\..\..\hook\pidexternalapi.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\klick\klick_api.h"\
	"..\..\..\klsdk\kldef.h"\
	"..\..\..\klsdk\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\klsdk\ntdef.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\klsdk\plugapi.h"\
	"..\..\..\klsdk\TDI.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"..\..\..\TRUE\FilterEvent.h"\
	"..\..\reg_plugin.h"\
	"..\alert.c"\
	"..\attacks.c"\
	"..\attacks.h"\
	"..\dos.c"\
	"..\Element.c"\
	"..\Flood.c"\
	"..\g_precomp.h"\
	"..\handlers.h"\
	"..\Parse.c"\
	"..\registry.h"\
	"..\Scan.c"\
	"..\stealth.c"\
	"..\STEALTH.H"\
	"..\trust.c"\
	"c:\ddk\ddknt4\inc\alpharef.h"\
	"c:\ddk\ddknt4\inc\bugcodes.h"\
	"c:\ddk\ddknt4\inc\exlevels.h"\
	"c:\ddk\ddknt4\inc\ntddk.h"\
	"c:\ddk\ddknt4\inc\ntdef.h"\
	"c:\ddk\ddknt4\inc\ntiologc.h"\
	"c:\ddk\ddknt4\inc\ntpoapi.h"\
	"c:\ddk\ddknt4\inc\ntstatus.h"\
	"C:\DDK\DDKNT4\src\network\inc\afilter.h"\
	"C:\DDK\DDKNT4\src\network\inc\efilter.h"\
	"C:\DDK\DDKNT4\src\network\inc\ffilter.h"\
	"c:\ddk\ddknt4\src\network\inc\ndis.h"\
	"c:\ddk\ddknt4\src\network\inc\netevent.h"\
	"C:\DDK\DDKNT4\src\network\inc\nettypes.h"\
	"c:\ddk\ddknt4\src\network\inc\ntddndis.h"\
	"C:\DDK\DDKNT4\src\network\inc\ntddtdi.h"\
	"C:\DDK\DDKNT4\src\network\inc\packoff.h"\
	"C:\DDK\DDKNT4\src\network\inc\packon.h"\
	"C:\DDK\DDKNT4\src\network\inc\tdi.h"\
	"C:\DDK\DDKNT4\src\network\inc\tdikrnl.h"\
	"C:\DDK\DDKNT4\src\network\inc\tfilter.h"\
	
NODEP_CPP_HANDL=\
	"..\..\..\klsdk\tdistat.h"\
	"..\Win9x\handlers_9x.c"\
	".\handlers_nt.c"\
	

!IF  "$(CFG)" == "klpid - Win32 Release"

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.c
DEP_CPP_MAIN_=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\netid.h"\
	"..\..\..\hook\pidexternalapi.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\klick\klick_api.h"\
	"..\..\..\klsdk\kldef.h"\
	"..\..\..\klsdk\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\klsdk\ntdef.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\klsdk\plugapi.h"\
	"..\..\..\klsdk\TDI.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"..\..\..\TRUE\FilterEvent.h"\
	"..\..\reg_plugin.h"\
	"..\attacks.h"\
	"..\g_precomp.h"\
	"..\handlers.h"\
	"..\registry.h"\
	"..\STEALTH.H"\
	".\precomp.h"\
	"c:\ddk\ddknt4\inc\alpharef.h"\
	"c:\ddk\ddknt4\inc\bugcodes.h"\
	"c:\ddk\ddknt4\inc\exlevels.h"\
	"c:\ddk\ddknt4\inc\ntddk.h"\
	"c:\ddk\ddknt4\inc\ntdef.h"\
	"c:\ddk\ddknt4\inc\ntiologc.h"\
	"c:\ddk\ddknt4\inc\ntpoapi.h"\
	"c:\ddk\ddknt4\inc\ntstatus.h"\
	"C:\DDK\DDKNT4\src\network\inc\afilter.h"\
	"C:\DDK\DDKNT4\src\network\inc\efilter.h"\
	"C:\DDK\DDKNT4\src\network\inc\ffilter.h"\
	"c:\ddk\ddknt4\src\network\inc\ndis.h"\
	"c:\ddk\ddknt4\src\network\inc\netevent.h"\
	"C:\DDK\DDKNT4\src\network\inc\nettypes.h"\
	"c:\ddk\ddknt4\src\network\inc\ntddndis.h"\
	"C:\DDK\DDKNT4\src\network\inc\ntddtdi.h"\
	"C:\DDK\DDKNT4\src\network\inc\packoff.h"\
	"C:\DDK\DDKNT4\src\network\inc\packon.h"\
	"C:\DDK\DDKNT4\src\network\inc\tdi.h"\
	"C:\DDK\DDKNT4\src\network\inc\tdikrnl.h"\
	"C:\DDK\DDKNT4\src\network\inc\tfilter.h"\
	
NODEP_CPP_MAIN_=\
	"..\..\..\klsdk\tdistat.h"\
	

!IF  "$(CFG)" == "klpid - Win32 Release"

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\reg_plugin.c
DEP_CPP_REG_P=\
	"c:\ddk\ddknt4\inc\alpharef.h"\
	"c:\ddk\ddknt4\inc\bugcodes.h"\
	"c:\ddk\ddknt4\inc\exlevels.h"\
	"c:\ddk\ddknt4\inc\ntddk.h"\
	"c:\ddk\ddknt4\inc\ntdef.h"\
	"c:\ddk\ddknt4\inc\ntiologc.h"\
	"c:\ddk\ddknt4\inc\ntpoapi.h"\
	"c:\ddk\ddknt4\inc\ntstatus.h"\
	

!IF  "$(CFG)" == "klpid - Win32 Release"

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\registry.c
DEP_CPP_REGIS=\
	"..\..\..\hook\avpgcom.h"\
	"..\..\..\hook\hookspec.h"\
	"..\..\..\hook\netid.h"\
	"..\..\..\hook\pidexternalapi.h"\
	"..\..\..\hook\singleparam.h"\
	"..\..\..\klick\klick_api.h"\
	"..\..\..\klsdk\kldef.h"\
	"..\..\..\klsdk\klstatus.h"\
	"..\..\..\klsdk\nettypes.h"\
	"..\..\..\klsdk\ntdef.h"\
	"..\..\..\klsdk\packoff.h"\
	"..\..\..\klsdk\packon.h"\
	"..\..\..\klsdk\plugapi.h"\
	"..\..\..\klsdk\TDI.h"\
	"..\..\..\klsdk\tdivxd.h"\
	"..\..\..\klsdk\w9xdef.h"\
	"..\..\..\TRUE\FilterEvent.h"\
	"..\..\reg_plugin.h"\
	"..\attacks.h"\
	"..\g_precomp.h"\
	"..\handlers.h"\
	"..\registry.h"\
	"..\STEALTH.H"\
	"c:\ddk\ddknt4\inc\alpharef.h"\
	"c:\ddk\ddknt4\inc\bugcodes.h"\
	"c:\ddk\ddknt4\inc\exlevels.h"\
	"c:\ddk\ddknt4\inc\ntddk.h"\
	"c:\ddk\ddknt4\inc\ntdef.h"\
	"c:\ddk\ddknt4\inc\ntiologc.h"\
	"c:\ddk\ddknt4\inc\ntpoapi.h"\
	"c:\ddk\ddknt4\inc\ntstatus.h"\
	"C:\DDK\DDKNT4\src\network\inc\afilter.h"\
	"C:\DDK\DDKNT4\src\network\inc\efilter.h"\
	"C:\DDK\DDKNT4\src\network\inc\ffilter.h"\
	"c:\ddk\ddknt4\src\network\inc\ndis.h"\
	"c:\ddk\ddknt4\src\network\inc\netevent.h"\
	"C:\DDK\DDKNT4\src\network\inc\nettypes.h"\
	"c:\ddk\ddknt4\src\network\inc\ntddndis.h"\
	"C:\DDK\DDKNT4\src\network\inc\ntddtdi.h"\
	"C:\DDK\DDKNT4\src\network\inc\packoff.h"\
	"C:\DDK\DDKNT4\src\network\inc\packon.h"\
	"C:\DDK\DDKNT4\src\network\inc\tdi.h"\
	"C:\DDK\DDKNT4\src\network\inc\tdikrnl.h"\
	"C:\DDK\DDKNT4\src\network\inc\tfilter.h"\
	
NODEP_CPP_REGIS=\
	"..\..\..\klsdk\tdistat.h"\
	

!IF  "$(CFG)" == "klpid - Win32 Release"

!ELSEIF  "$(CFG)" == "klpid - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\g_precomp.h
# End Source File
# Begin Source File

SOURCE=..\handlers.h
# End Source File
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# Begin Source File

SOURCE=..\..\reg_plugin.h
# End Source File
# Begin Source File

SOURCE=..\registry.h
# End Source File
# End Group
# End Target
# End Project

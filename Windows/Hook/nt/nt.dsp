# Microsoft Developer Studio Project File - Name="nt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=nt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nt.mak" CFG="nt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "nt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/nt", AXZAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "..\..\..\temp\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /D "WIN32" /D "_WINDOWS" /Oxs /c
# ADD CPP /nologo /G5 /Gz /MT /W3 /Zi /Ox /Oi /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\ddk" /I "$(CPU)\\" /I "..\..\..\CommonFiles" /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=1 /D "_IDWBUILD" /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "_HOOK_NT_" /D "_HOOK_IMPERSONATE_" /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /Gf
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"..\Release/klif.pdb" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"..\Release\klif.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"$(BASEDIR)\libfre\i386" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x1000 /osversion:4.00 /subsystem:native /SECTION:.user,ER /MERGE:.userd=.user /MERGE:.userc=.user
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=PostBuild step
PostBuild_Cmds=PostBuild.cmd Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "..\..\..\temp\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MTd /W3 /Zi /Oi /Gy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\inc\ddk" /I "$(CPU)\\" /I "..\..\..\CommonFiles" /FI"$(BASEDIR)\inc\warning.h" /D WIN32=100 /D "_DEBUG" /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D _WIN32_WINNT=0x0500 /D WIN32_LEAN_AND_MEAN=1 /D DBG=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D "_HOOK_NT_" /D "_HOOK_IMPERSONATE_" /Fr /FD /Zel -cbstring /QIfdiv- /QIf /GF /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "\..\..\CommonFiles" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /incremental:no /pdb:"..\Debug\klif.pdb" /map /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"..\Debug\klif.sys" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"$(BASEDIR)\libchk\i386" /driver /debug:notmapped,FULL /IGNORE:4001,4037,4039,4065,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x1000 /osversion:4.00 /subsystem:native /SECTION:.user,ER,ALIGN=4096 /MERGE:.userd=.user /MERGE:.userc=.user
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=extract dbg
PostBuild_Cmds=PostBuild.cmd Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "nt - Win32 Release"
# Name "nt - Win32 Debug"
# Begin Source File

SOURCE=..\client.c
DEP_CPP_CLIEN=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\evqueue.h"\
	"..\hook\avpgcom.h"\
	"..\klsecur.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\glbenvir.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_CLIEN=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\client.h
# End Source File
# Begin Source File

SOURCE=..\debug.c
DEP_CPP_DEBUG=\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\osspec.h"\
	"..\structures.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_DEBUG=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\diskio.c

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\evqueue.c
DEP_CPP_EVQUE=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\evqueue.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\klsecur.h"\
	"..\osspec.h"\
	"..\structures.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_EVQUE=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\evqueue.h
# End Source File
# Begin Source File

SOURCE=.\fastio.c

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fidbox.c
DEP_CPP_FIDBO=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\InvThread.h"\
	"..\klsecur.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\fidbox.h"\
	".\glbenvir.h"\
	".\objidindex.h"\
	".\specfunc.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_FIDBO=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\fidbox.h
# End Source File
# Begin Source File

SOURCE=.\fileio.c

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\filter.c
DEP_CPP_FILTE=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\InvThread.h"\
	"..\lists.h"\
	"..\osspec.h"\
	"..\PPage.c"\
	"..\structures.h"\
	".\runproc.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_FILTE=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\filter.h
# End Source File
# Begin Source File

SOURCE=.\firewall.c

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\glbenvir.c
DEP_CPP_GLBEN=\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\glbenvir.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_GLBEN=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\glbenvir.h
# End Source File
# Begin Source File

SOURCE=..\InvThread.c
DEP_CPP_INVTH=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\InvThread.h"\
	"..\lists.h"\
	"..\osspec.h"\
	"..\structures.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_INVTH=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\InvThread.h
# End Source File
# Begin Source File

SOURCE=..\klsecur.c
DEP_CPP_KLSEC=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\InvThread.h"\
	"..\klsecur.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\glbenvir.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_KLSEC=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\klsecur.h
# End Source File
# Begin Source File

SOURCE=..\lists.h
# End Source File
# Begin Source File

SOURCE=.\LLDiskIO.c
DEP_CPP_LLDIS=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\NameCache.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\glbenvir.h"\
	".\LLDiskIO.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdddisk.h"\
	"C:\winddk\DDKNT4\inc\ntddstor.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_LLDIS=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LLDiskIO.h
# End Source File
# Begin Source File

SOURCE=..\md5.c
DEP_CPP_MD5_C=\
	"..\md5.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\md5.h
# End Source File
# Begin Source File

SOURCE=..\NameCache.c
DEP_CPP_NAMEC=\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\NameCache.h"\
	"..\osspec.h"\
	"..\structures.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_NAMEC=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\NameCache.h
# End Source File
# Begin Source File

SOURCE=.\nt.dep
# End Source File
# Begin Source File

SOURCE=.\nt.mak
# End Source File
# Begin Source File

SOURCE=.\nthook.c
DEP_CPP_NTHOO=\
	"..\client.h"\
	"..\COMMON.C"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\evqueue.h"\
	"..\filter.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\hook\klick_api.h"\
	"..\HOOK\klmc_api.h"\
	"..\HOOK\klpf_api.h"\
	"..\HOOK\minikav_api.h"\
	"..\HOOK\PIDExternalAPI.H"\
	"..\InvThread.h"\
	"..\ioctl.c"\
	"..\kldefs.h"\
	"..\KLSDK\baseapi.h"\
	"..\KLSDK\klstatus.h"\
	"..\klsecur.h"\
	"..\logging.c"\
	"..\NameCache.h"\
	"..\Ntcommon.cpp"\
	"..\osspec.h"\
	"..\r3.h"\
	"..\structures.h"\
	"..\syscalls.h"\
	"..\sysnamecache.h"\
	"..\TSPv2.h"\
	".\diskio.c"\
	".\fastio.c"\
	".\fidbox.h"\
	".\fileio.c"\
	".\firewall.c"\
	".\glbenvir.h"\
	".\LLDiskIO.h"\
	".\registry.c"\
	".\runproc.h"\
	".\specfunc.h"\
	".\SysIO.h"\
	".\useklin.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdddisk.h"\
	"C:\winddk\DDKNT4\inc\ntddstor.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_NTHOO=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\nthook.rc
# End Source File
# Begin Source File

SOURCE=.\objidindex.c
DEP_CPP_OBJID=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\InvThread.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\fidbox.h"\
	".\objidindex.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_OBJID=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objidindex.h
# End Source File
# Begin Source File

SOURCE=..\osspec.c
DEP_CPP_OSSPE=\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\NameCache.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\glbenvir.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_OSSPE=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\osspec.h
# End Source File
# Begin Source File

SOURCE=.\PostBuild.cmd
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\r3.c
DEP_CPP_R3_C1c=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\evqueue.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\InvThread.h"\
	"..\kldefs.h"\
	"..\klsecur.h"\
	"..\osspec.h"\
	"..\r3.h"\
	"..\structures.h"\
	"..\syscalls.h"\
	"..\TSPv2.h"\
	".\glbenvir.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_R3_C1c=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\r3.h
# End Source File
# Begin Source File

SOURCE=.\registry.c

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\runproc.c
DEP_CPP_RUNPR=\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\lists.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\runproc.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_RUNPR=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\runproc.h
# End Source File
# Begin Source File

SOURCE=.\specfunc.c
DEP_CPP_SPECF=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\InvThread.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\specfunc.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_SPECF=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\specfunc.h
# End Source File
# Begin Source File

SOURCE=..\syscalls.c
DEP_CPP_SYSCA=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\evqueue.h"\
	"..\hook\avpgcom.h"\
	"..\InvThread.h"\
	"..\klsecur.h"\
	"..\osspec.h"\
	"..\structures.h"\
	"..\syscalls.h"\
	"..\TSPv2.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_SYSCA=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\syscalls.h
# End Source File
# Begin Source File

SOURCE=.\SysIO.c
DEP_CPP_SYSIO=\
	"..\client.h"\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\evqueue.h"\
	"..\filter.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\InvThread.h"\
	"..\kldefs.h"\
	"..\klsecur.h"\
	"..\lists.h"\
	"..\md5.h"\
	"..\NameCache.h"\
	"..\osspec.h"\
	"..\r3.h"\
	"..\structures.h"\
	"..\syscalls.h"\
	"..\sysnamecache.h"\
	"..\TSPv2.h"\
	".\fidbox.h"\
	".\glbenvir.h"\
	".\runproc.h"\
	".\SysIO.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_SYSIO=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SysIO.h
# End Source File
# Begin Source File

SOURCE=..\sysnamecache.c
DEP_CPP_SYSNA=\
	"..\debug.h"\
	"..\defs.h"\
	"..\lists.h"\
	"..\sysnamecache.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_SYSNA=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\sysnamecache.h
# End Source File
# Begin Source File

SOURCE=..\tspv2.c
DEP_CPP_TSPV2=\
	"..\debug.h"\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\osspec.h"\
	"..\structures.h"\
	"..\TSPv2.h"\
	".\glbenvir.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_TSPV2=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\TSPv2.h
# End Source File
# Begin Source File

SOURCE=.\useklin.c
DEP_CPP_USEKL=\
	"..\defs.h"\
	"..\drvtypes.h"\
	"..\hook\avpgcom.h"\
	"..\hook\hookspec.h"\
	"..\hook\klin_api.h"\
	"..\KLSDK\baseapi.h"\
	"..\KLSDK\klstatus.h"\
	"..\osspec.h"\
	"..\structures.h"\
	".\useklin.h"\
	"C:\winddk\DDKNT4\inc\alpharef.h"\
	"C:\winddk\DDKNT4\inc\bugcodes.h"\
	"C:\winddk\DDKNT4\inc\exlevels.h"\
	"C:\winddk\DDKNT4\inc\ntdef.h"\
	"C:\winddk\DDKNT4\inc\NTIFS.H"\
	"C:\winddk\DDKNT4\inc\ntiologc.h"\
	"C:\winddk\DDKNT4\inc\ntnls.h"\
	"C:\winddk\DDKNT4\inc\ntpoapi.h"\
	"C:\winddk\DDKNT4\inc\ntstatus.h"\
	
NODEP_CPP_USEKL=\
	"..\vtoolsc.h"\
	

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\useklin.h
# End Source File
# Begin Source File

SOURCE=.\VER_MOD.h
# End Source File
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="kl1_nt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=KL1_NT - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KL1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KL1.mak" CFG="KL1_NT - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kl1_nt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kl1_nt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Windows/Hook/KL1/winnt", TPIEBAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../temp/release/kl1_nt"
# PROP Intermediate_Dir "../../../../temp/release/kl1_nt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /WX /Oy /Gy /D "WIN32" /Oxs /c
# ADD CPP /nologo /Gz /W3 /Zi /O2 /Oy /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\KLSDK" /I "..\.." /I "$(VTOOLSD)\include" /I "..\..\..\..\CommonFiles\Sign" /FI"$(BASEDIR)\inc\warning.h" /D "_WINNT" /D "_CONSOLE" /D BINARY_COMPATIBLE=0 /D i386=1 /D DBG=1 /D "__CPP" /D _DLL=1 /D _X86_=1 /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D WIN32=100 /D _NT1X_=100 /D WIN32_LEAN_AND_MEAN=1 /D "NDIS40" /D "NDIS40_MINIPORT" /D "NDIS_MINIPORT_DRIVER" /Oxs /Zel -cbstring /QIfdiv- /QIf /GF /c
# SUBTRACT CPP /WX /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\..\..\..\CommonFiles" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib ndis.lib tdi.lib sign.lib /nologo /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"../../../../out/release/kl1.pdb" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"../../../../out/release/kl1.sys" /libpath:"$(BASEDIR)\lib\i386\free" /libpath:"..\..\..\..\out\release" /driver /debug:notmapped,MINIMAL /IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../temp/debug/kl1_nt"
# PROP Intermediate_Dir "../../../../temp/debug/kl1_nt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /Gz /W3 /Zi /Od /D "WIN32" /YX /FD /c
# ADD CPP /nologo /Gz /W3 /Zi /Oi /Gy /I "..\..\..\..\CommonFiles\Sign" /I "$(BASEDIR)\inc" /I "$(BASEDIR)\src\network\inc" /I "..\..\KLSDK" /I "..\.." /I "$(VTOOLSD)\include" /D "__CPP" /D "_DEBUG" /D DBG=1 /D "_UNICODE" /D WIN32=100 /D "_WINDOWS" /D "STD_CALL" /D CONDITION_HANDLING=1 /D NT_UP=1 /D NT_INST=0 /D _NT1X_=100 /D WINNT=1 /D WIN32_LEAN_AND_MEAN=1 /D DEVL=1 /D FPO=0 /D "NDEBUG" /D _DLL=1 /D _X86_=1 /D $(CPU)=1 /D "NDIS_MINIPORT_DRIVER" /D "NDIS40" /D "NDIS40_MINIPORT" /D BINARY_COMPATIBLE=0 /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "$(BASEDIR)\inc" /i "..\..\..\..\CommonFiles"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"objchk\i386\kl1_nt.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /machine:IX86
# ADD LINK32 int64.lib ntoskrnl.lib hal.lib ndis.lib tdi.lib sign.lib /base:"0x10000" /version:4.0 /entry:"DriverEntry" /pdb:"../../../../out/debug/kl1.pdb" /debug /debugtype:both /machine:IX86 /nodefaultlib /out:"../../../../out/debug/kl1.sys" /libpath:"$(BASEDIR)\lib\i386\checked" /libpath:"..\..\..\..\out\debug" /driver /debug:notmapped,FULL /IGNORE:4006,4001,4037,4039,4065,4078,4087,4088,4089,4096 /MERGE:_PAGE=PAGE /MERGE:_TEXT=.text /SECTION:INIT,d /MERGE:.rdata=.text /FULLBUILD /RELEASE /FORCE:MULTIPLE /OPT:REF /OPTIDATA /align:0x20 /osversion:4.00 /subsystem:native
# SUBTRACT LINK32 /nologo /pdb:none /incremental:no

!ENDIF 

# Begin Target

# Name "kl1_nt - Win32 Release"
# Name "kl1_nt - Win32 Debug"
# Begin Group "source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\KLSDK\kl_registry.cpp
DEP_CPP_KL_RE=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kl_registry.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\klload.cpp
DEP_CPP_KLLOA=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\klload.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\loader_nt.cpp
DEP_CPP_LOADE=\
	"..\..\..\..\CommonFiles\Sign\sign.h"\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\g_registry.h"\
	"..\..\KLSDK\kl_file.h"\
	"..\..\KLSDK\kl_hook.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\pe.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\klload.h"\
	"..\loader.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp
DEP_CPP_MAIN_=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\HOOK\kl1_api.h"\
	"..\..\KLSDK\g_registry.h"\
	"..\..\KLSDK\kl_file.h"\
	"..\..\KLSDK\kl_hook.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kl_registry.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\pe.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"..\klload.h"\
	"..\loader.h"\
	".\main.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\HOOK\kl1_api.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_registry.h
# End Source File
# Begin Source File

SOURCE=..\klload.h
# End Source File
# Begin Source File

SOURCE=..\loader.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# End Group
# Begin Group "KLSDK source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\KLSDK\g_registry.cpp
DEP_CPP_G_REG=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\g_registry.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_file.cpp
DEP_CPP_KL_FI=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kl_file.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_hook.cpp
DEP_CPP_KL_HO=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kl_hook.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\pe.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_list.cpp
DEP_CPP_KL_LI=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_lock.cpp
DEP_CPP_KL_LO=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_object.cpp
DEP_CPP_KL_OB=\
	"..\..\debug.h"\
	"..\..\defs.h"\
	"..\..\KLSDK\kl_debug.h"\
	"..\..\KLSDK\kl_DebugMask.h"\
	"..\..\KLSDK\kl_list.h"\
	"..\..\KLSDK\kl_lock.h"\
	"..\..\KLSDK\kl_object.h"\
	"..\..\KLSDK\kldef.h"\
	"..\..\KLSDK\klstatus.h"\
	"..\..\KLSDK\nettypes.h"\
	"..\..\KLSDK\nt_def.h"\
	"..\..\KLSDK\packoff.h"\
	"..\..\KLSDK\packon.h"\
	"..\..\KLSDK\ring3def.h"\
	"..\..\KLSDK\TDI.h"\
	"..\..\KLSDK\tdivxd.h"\
	"..\..\KLSDK\w9xdef.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\aep.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcdefseg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\bcintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\blockdev.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\configmg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\crtl.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\dcb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ddkinc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\drp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ifs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ilb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\iop.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\ior.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\irs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\isp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\msintrin.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pagefile.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pccard.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\pipevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\power.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\scsiport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\sgd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\shell.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\srb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\tdistat.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\v86mmgr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vappy.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcache.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcallbak.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vcomport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vd2wpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vddgrb.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdebug.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevapi.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdevice.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdispatc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdma.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdmad.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vdpmient.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vevent.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfault.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vfbackup.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhotkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vhwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vid.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vintrs.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vipf.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vkd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\VKDpaste.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vlist.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmachine.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmcpd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmemory.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmm.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmmreg.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmpoll.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vmutex.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpicd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpipe.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vport.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vpowerd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vregkey.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vrp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsemapho.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vshwint.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vsvctab.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthread.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vthunks.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtimeout.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsc.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolscp.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vtoolsd.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vwin32.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdldr.h"\
	"C:\PROGRA~2\NuMega\DRIVER~1\VtoolsD\include\vxdsvc.h"\
	"C:\Work\DDK\nt40\inc\alpharef.h"\
	"C:\Work\DDK\nt40\inc\bugcodes.h"\
	"C:\Work\DDK\nt40\inc\exlevels.h"\
	"C:\Work\DDK\nt40\inc\miniport.h"\
	"C:\Work\DDK\nt40\inc\ntddk.h"\
	"C:\Work\DDK\nt40\inc\ntdef.h"\
	"C:\Work\DDK\nt40\inc\NTIFS.H"\
	"C:\Work\DDK\nt40\inc\ntiologc.h"\
	"C:\Work\DDK\nt40\inc\ntnls.h"\
	"C:\Work\DDK\nt40\inc\ntpoapi.h"\
	"C:\Work\DDK\nt40\inc\ntstatus.h"\
	"C:\Work\DDK\nt40\src\network\inc\afilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\efilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ffilter.h"\
	"C:\Work\DDK\nt40\src\network\inc\ndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\netevent.h"\
	"C:\Work\DDK\nt40\src\network\inc\nettypes.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddndis.h"\
	"C:\Work\DDK\nt40\src\network\inc\ntddtdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\packoff.h"\
	"C:\Work\DDK\nt40\src\network\inc\packon.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdi.h"\
	"C:\Work\DDK\nt40\src\network\inc\tdikrnl.h"\
	"C:\Work\DDK\nt40\src\network\inc\tfilter.h"\
	

!IF  "$(CFG)" == "kl1_nt - Win32 Release"

!ELSEIF  "$(CFG)" == "kl1_nt - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "KLSDK include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\KLSDK\g_registry.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_file.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_hook.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_list.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_lock.h
# End Source File
# Begin Source File

SOURCE=..\..\KLSDK\kl_object.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\kl1_nt.rc
# End Source File
# End Target
# End Project

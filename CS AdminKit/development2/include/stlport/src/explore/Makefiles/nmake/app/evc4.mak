# -*- makefile -*- Time-stamp: <04/03/31 08:08:12 ptr>
# $Id: evc4.mak,v 1.1.2.1 2004/10/05 18:41:54 dums Exp $

LDFLAGS_COMMON = commctrl.lib coredll.lib corelibc.lib ccrtrtti.lib /nodefaultlib:"OLDNAMES.lib" /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /nodefaultlib:"libc.lib" /subsystem:WINDOWSCE /align:"4096"

!if "$(TARGET_PROC)" == "arm"
LDFLAGS_COMMON = $(LDFLAGS_COMMON) /MACHINE:ARM
!endif

!if "$(TARGET_PROC)" == "x86"
LDFLAGS_COMMON = $(LDFLAGS_COMMON) $(CEx86Corelibc) /MACHINE:IX86
!endif

!include evc-common.mak

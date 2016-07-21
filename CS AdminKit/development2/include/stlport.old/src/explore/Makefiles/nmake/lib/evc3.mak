# -*- makefile -*- Time-stamp: <04/05/01 00:34:42 ptr>
# $Id: evc3.mak,v 1.1.2.2 2004/10/05 18:42:25 dums Exp $

LDFLAGS_COMMON = commctrl.lib coredll.lib corelibc.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /incremental:no /nodefaultlib:"LIBC.lib" /subsystem:WINDOWSCE /align:"4096"

!if "$(TARGET_PROC)" == "arm"
LDFLAGS_COMMON = $(LDFLAGS_COMMON) /MACHINE:ARM
!endif

!if "$(TARGET_PROC)" == "x86"
LDFLAGS_COMMON = $(LDFLAGS_COMMON) $(CEx86Corelibc) /nodefaultlib:"OLDNAMES.lib" /MACHINE:X86
!endif

!include evc-common.mak

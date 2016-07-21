# -*- Makefile -*- Time-stamp: <04/03/31 08:14:15 ptr>
# $Id: nmake-evc4.mak,v 1.1.2.1 2004/10/05 18:43:45 dums Exp $

COMPILER_NAME=evc4
CEVERSION=420

!if "$(TARGET_PROC)" == "arm"
OPT_STLDBG_STATIC = /Zm800
!endif

!include nmake-evc-common.mak

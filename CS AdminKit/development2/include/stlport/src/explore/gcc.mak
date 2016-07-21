# -*- Makefile -*- Time-stamp: <03/10/12 20:35:49 ptr>
# $Id: gcc.mak,v 1.3 2004/03/08 20:36:57 dums Exp $

SRCROOT := .
COMPILER_NAME := gcc

STLPORT_INCLUDE_DIR = ../../stlport
include Makefile.inc
include ${SRCROOT}/Makefiles/top.mak


INCLUDES += -I$(STLPORT_INCLUDE_DIR)

ifeq ($(OSNAME),linux)
DEFS += -D_GNU_SOURCE
endif

# options for build with boost support
ifdef STLP_BUILD_BOOST_PATH
INCLUDES += -I$(STLP_BUILD_BOOST_PATH)
endif


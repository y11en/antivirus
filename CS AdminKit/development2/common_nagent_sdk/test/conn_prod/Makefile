# -*- Makefile -*- Time-stamp: <03/09/25 12:02:31 ptr>
# $Id: gcc-libstd.mak,v 1.1 2004/04/09 14:34:51 ptr Exp $

SRCROOT := .
COMPILER_NAME := gcc
# STLPORT_INCLUDE_DIR := ${SRCROOT}/include/stlport/stlport

ALL_TAGS = release-shared 

# dbg-shared

include Makefile.inc
include ${SRCROOT}/Makefiles/top.mak

DEFS += -DUNICODE -DUSE_BOOST_THREADS
INCLUDES += -I$(SRCROOT)/../../include

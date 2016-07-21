# Time-stamp: <03/07/15 17:54:22 ptr>
# $Id: macro.mak,v 1.1.2.2 2004/10/13 10:11:56 ptr Exp $

PRG        := $(OUTPUT_DIR)/${PRGNAME}${EXE}
PRG_DBG    := $(OUTPUT_DIR_DBG)/${PRGNAME}${EXE}
PRG_STLDBG := $(OUTPUT_DIR_STLDBG)/${PRGNAME}${EXE}

LDFLAGS += ${LDSEARCH}

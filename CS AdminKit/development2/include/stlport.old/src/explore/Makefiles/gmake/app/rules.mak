# -*- makefile -*- Time-stamp: <04/07/25 13:09:24 ptr>
# $Id: rules.mak,v 1.1.2.1 2004/07/26 11:17:22 ptr Exp $

dbg-shared:	$(OUTPUT_DIR_DBG) ${PRG_DBG}

stldbg-shared:	$(OUTPUT_DIR_STLDBG) ${PRG_STLDBG}

release-shared:	$(OUTPUT_DIR) ${PRG}

${PRG}:	$(OBJ) $(LIBSDEP)
	$(LINK.cc) $(LINK_OUTPUT_OPTION) ${START_OBJ} $(OBJ) ${END_OBJ} $(LDLIBS) ${STDLIBS}

${PRG_DBG}:	$(OBJ_DBG) $(LIBSDEP)
	$(LINK.cc) $(LINK_OUTPUT_OPTION) ${START_OBJ} $(OBJ_DBG) ${END_OBJ} $(LDLIBS) ${STDLIBS}

${PRG_STLDBG}:	$(OBJ_STLDBG) $(LIBSDEP)
	$(LINK.cc) $(LINK_OUTPUT_OPTION) ${START_OBJ} $(OBJ_STLDBG) ${END_OBJ} $(LDLIBS) ${STDLIBS}


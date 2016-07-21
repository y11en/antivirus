# Time-stamp: <03/07/15 12:20:52 ptr>
# $Id: depend.mak,v 1.1 2003/11/14 19:58:28 ptr Exp $

PHONY += release-static-dep release-shared-dep dbg-static-dep dbg-shared-dep \
         stldbg-static-dep stldbg-shared-dep depend

release-static-dep release-shared-dep:	$(DEP)

dbg-static-dep dbg-shared-dep:	$(DEP_DBG)

stldbg-static-dep stldbg-shared-dep:	$(DEP_STLDBG)

depend:	dirs release-shared-dep dbg-shared-dep stldbg-shared-dep
	@cat -s $(DEP) $(DEP_DBG) $(DEP_STLDBG) /dev/null > $(DEPENDS_COLLECTION)

-include $(DEPENDS_COLLECTION)

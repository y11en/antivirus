# -*- makefile -*- Time-stamp: <03/07/10 00:20:54 ptr>
# $Id: clean.mak,v 1.1.2.2 2004/10/13 10:11:56 ptr Exp $

clobber::
	@-rm -f ${PRG}
	@-rm -f ${PRG_DBG}
	@-rm -f ${PRG_STLDBG}

distclean::
	@-rm -f $(INSTALL_BIN_DIR)/$(PRG)
	@-rm -f $(INSTALL_BIN_DIR_DBG)/$(PRG_DBG)
	@-rm -f $(INSTALL_BIN_DIR_STLDBG)/$(PRG_STLDBG)

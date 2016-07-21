# Time-stamp: <03/10/17 19:29:26 ptr>
# $Id: rules-install-a.mak,v 1.1.2.1 2004/09/23 19:28:10 dums Exp $

install-release-static: release-static $(INSTALL_STATIC_BIN_DIR)
	$(INSTALL_A) $(PRG_A) $(INSTALL_STATIC_BIN_DIR)

install-dbg-static: dbg-static $(INSTALL_STATIC_BIN_DIR_DBG)
	$(INSTALL_A) $(PRG_A_DBG) $(INSTALL_STATIC_BIN_DIR_DBG)

install-stldbg-static: stldbg-static $(INSTALL_STATIC_BIN_DIR_STLDBG)
	$(INSTALL_A) $(PRG_A_STLDBG) $(INSTALL_STATIC_BIN_DIR_STLDBG)

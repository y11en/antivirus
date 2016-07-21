#if !defined(PPP_CMDLN_H)
#define PPP_CMDLN_H

// Ключи командной строки

#define KAV_CMDLN_BL          _T("bl")
#define KAV_CMDLN_GUI         _T("gui")

#define KAV_CMDLN_INSTALL     _T("i")
#define KAV_CMDLN_UNISTALL    _T("u")
#define KAV_CMDLN_LOGIN       _T("l")
#define KAV_CMDLN_PASSWORD    _T("p")
#define KAV_CMDLN_START       _T("s")
#define KAV_CMDLN_EXIT        _T("e")
#define KAV_CMDLN_DEBUG       _T("d")
#define KAV_CMDLN_DEBUG_WAIT  _T("dw")
#define KAV_CMDLN_NO_EXCEPTIONS  _T("ne")

#define KAV_CMDLN_HOST        _T("host")
#define KAV_CMDLN_RUN_SERVICE _T("r")

#define KAV_CMDLN_CONSOLE     _T("con")
#define KAV_CMDLN_SHELL       _T("shell")
#define KAV_CMDLN_HIDDEN      _T("hidden") // don't create console window while executing shell command
#define KAV_CMDLN_WAIT        _T("wait")   // show "Press any key..." in console window

#define KAV_CMDLN_DONOTSTARTSELFPROTECTION _T("nsp")

#endif// PPP_CMDLN_H

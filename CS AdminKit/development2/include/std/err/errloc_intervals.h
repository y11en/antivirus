
/*KLCSAK_PUBLIC_HEADER*/

#ifndef __ERRLOC_INTERVALS_H__
#define __ERRLOC_INTERVALS_H__

#define KLERR_LOCMOD_BASE       (50000)

#define KLERR_LOCMOD_STD            L"KLSTD"
#define KLERR_LOCMOD_BASE_STD       (KLERR_LOCMOD_BASE)
#define KLERR_LOCMOD_TRAP           L"TRAP"
#define KLERR_LOCMOD_BASE_TRAP      (KLERR_LOCMOD_BASE + 100)
#define KLERR_LOCMOD_PRCI           L"KLPRCI"
#define KLERR_LOCMOD_BASE_PRCI      (KLERR_LOCMOD_BASE + 200)
#define KLERR_LOCMOD_PRCP           L"KLPRCP"
#define KLERR_LOCMOD_BASE_PRCP      (KLERR_LOCMOD_BASE + 300)
#define KLERR_LOCMOD_AGINST         L"KLAGINST"
#define KLERR_LOCMOD_BASE_AGINST    (KLERR_LOCMOD_BASE + 400)
#define KLERR_LOCMOD_NAG            L"KLNAG"
#define KLERR_LOCMOD_BASE_NAG       (KLERR_LOCMOD_BASE + 500)
#define KLERR_LOCMOD_PKG            L"KLPKG"
#define KLERR_LOCMOD_BASE_PKG       (KLERR_LOCMOD_BASE + 600)
#define KLERR_LOCMOD_HSTGRP         L"HSTGRP"
#define KLERR_LOCMOD_BASE_HSTGRP    (KLERR_LOCMOD_BASE + 700)
#define KLERR_LOCMOD_PRSS           L"KLPRSS"
#define KLERR_LOCMOD_BASE_PRSS      (KLERR_LOCMOD_BASE + 800)
#define KLERR_LOCMOD_CONN           L"KLCONN"
#define KLERR_LOCMOD_BASE_CONN      (KLERR_LOCMOD_BASE + 900)
#define KLERR_LOCMOD_KLBACKUP       L"KLBCKP"
#define KLERR_LOCMOD_BASE_KLBACKUP  (KLERR_LOCMOD_BASE + 1000)
#define KLERR_LOCMOD_KLOLA          L"KLOLA"
#define KLERR_LOCMOD_BASE_KLOLA     (KLERR_LOCMOD_BASE + 1100)


namespace KLERR
{
    struct ErrLocModule
    {
        const wchar_t*  szwModule;
        unsigned        nBaseId;
    };
    

    const KLERR::ErrLocModule c_LocModules[] = 
    {
        {KLERR_LOCMOD_STD,              KLERR_LOCMOD_BASE_STD},
        {KLERR_LOCMOD_TRAP,             KLERR_LOCMOD_BASE_TRAP},
        {KLERR_LOCMOD_PRCI,             KLERR_LOCMOD_BASE_PRCI},
        {KLERR_LOCMOD_PRCP,             KLERR_LOCMOD_BASE_PRCP},
        {KLERR_LOCMOD_AGINST,           KLERR_LOCMOD_BASE_AGINST},
        {KLERR_LOCMOD_NAG,              KLERR_LOCMOD_BASE_NAG},
        {KLERR_LOCMOD_PKG,              KLERR_LOCMOD_BASE_PKG},
        {KLERR_LOCMOD_HSTGRP,           KLERR_LOCMOD_BASE_HSTGRP},
        {KLERR_LOCMOD_PRSS,             KLERR_LOCMOD_BASE_PRSS},
        {KLERR_LOCMOD_CONN,             KLERR_LOCMOD_BASE_CONN},
        {KLERR_LOCMOD_KLBACKUP,         KLERR_LOCMOD_BASE_KLBACKUP},
        {KLERR_LOCMOD_KLOLA,            KLERR_LOCMOD_BASE_KLOLA}
    };
};

KLSTD_NOTHROW KLCSC_DECL void KLERR_LoadModuleLocalizations(
                                const wchar_t*              szwFileName,
                                const KLERR::ErrLocModule*  pData,
                                size_t                      nData) throw();

KLSTD_NOTHROW KLCSC_DECL void KLERR_UnloadModuleLocalizations(
                                const KLERR::ErrLocModule*  pData,
                                size_t                      nData) throw();

#endif //__ERRLOC_INTERVALS_H__
